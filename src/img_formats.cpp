#include "img_formats.h"
#include "rgba_pixel.h"


#define NYX_BENCH 1


#ifdef NYX_USE_OPENCL
#define NYX_CL_USE_FLOAT_BUFFERS 1
static const char* __kernel_yuv420p_to_rgba_f = "kernel void yuv420p_to_rgba(global uchar* srcY, uint strideY, global uchar* srcU, uint strideU, global uchar* srcV, uint strideV, uint w, uint h, write_only image2d_t dst)\
{\
	uint gx = get_global_id(0);\
	uint gy = get_global_id(1);\
\
	if ((gx < w) && (gy < h))\
	{\
		float4 p;\
\
		float Y = 1.1643 * (srcY[gx + gy * strideY] / 255.0f - 0.0625);\
		float Cr = srcU[gx / 2 + (gy / 2) * (strideU)] / 255.0f - 0.5f;\
		float Cb = srcV[gx / 2 + (gy / 2) * (strideV)] / 255.0f - 0.5f;\
\
		p.s0 = Y + 1.5958 * Cb;\
		p.s1 = Y - 0.39173 * Cr - 0.81290 * Cb;\
		p.s2 = Y + 2.017 * Cr;\
		p.s3 = 1.0f;\
\
		write_imagef(dst, (int2){gx, gy}, p);\
	}\
}";

static const char* __kernel_yuv420p_to_rgba_ui = "kernel void yuv420p_to_rgba(global uchar* srcY, uint strideY, global uchar* srcU, uint strideU, global uchar* srcV, uint strideV, uint w, uint h, write_only image2d_t dst)\
{\
	uint gx = get_global_id(0);\
	uint gy = get_global_id(1);\
\
	if ((gx < w) && (gy < h))\
	{\
		float4 p;\
		uint4 tt;\
\
		float Y = 1.1643 * (srcY[gx + gy * strideY] / 255.0f - 0.0625);\
		float Cr = srcU[gx / 2 + (gy / 2) * (strideU)] / 255.0f - 0.5f;\
		float Cb = srcV[gx / 2 + (gy / 2) * (strideV)] / 255.0f - 0.5f;\
\
		p.s0 = Y + 1.5958 * Cb;\
		p.s1 = Y - 0.39173 * Cr - 0.81290 * Cb;\
		p.s2 = Y + 2.017 * Cr;\
		p.s3 = 1.0f;\
\
		uint tmp = (uint)(p.s0 * 255.0);\
		tt.s0 = clamp(tmp, uint(0), uint(255));\
		tmp = (uint)(p.s1 * 255.0);\
		tt.s1 = clamp(tmp, uint(0), uint(255));\
		tmp = (uint)(p.s2 * 255.0);\
		tt.s2 = clamp(tmp, uint(0), uint(255));\
		tt.s3 = 255;\
\
		write_imageui(dst, (int2){gx, gy}, tt);\
	}\
}";
#endif /* NYX_USE_OPENCL */


img_format_t::img_format_t(const size_t width, const size_t height)
{
#ifdef NYX_USE_FFMPEG
	_sws_ctx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_RGBA, 0, NULL, NULL, NULL);
	avpicture_alloc(&_picture, AV_PIX_FMT_RGBA, width, height);
#endif /* NYX_USE_FFMPEG */

#ifdef NYX_USE_OPENCL
	_cl_device_id = nyx_cl_get_deviceid();
	_cl_context = nyx_cl_get_context();
	_cl_commands = nyx_cl_get_commandqueue();

	// create the compute program from the source buffer
	cl_int err;
#ifdef NYX_CL_USE_FLOAT_BUFFERS
	_cl_program = clCreateProgramWithSource(_cl_context, 1, (const char**)&__kernel_yuv420p_to_rgba_f, NULL, &err);
#else
	_cl_program = clCreateProgramWithSource(_cl_context, 1, (const char**)&__kernel_yuv420p_to_rgba_ui, NULL, &err);
#endif /* NYX_CL_USE_FLOAT_BUFFERS */
	if (!_cl_program)
	{
		NYX_ERRLOG("[!] Error: Failed to create compute program (%d)\n", err);
	}

	// build the program executable
	err = clBuildProgram(_cl_program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		size_t len = 0;
		char reason[2048] = {0x00};
		clGetProgramBuildInfo(_cl_program, _cl_device_id, CL_PROGRAM_BUILD_LOG, sizeof(reason), reason, &len);
		NYX_ERRLOG("[!] Error: Failed to build program executable (%d):\n%s", err, reason);
	}

	// create the compute kernel in the program we wish to run
	_cl_kernel = clCreateKernel(_cl_program, "yuv420p_to_rgba", &err);
	if ((!_cl_kernel) || (err != CL_SUCCESS))
	{
		NYX_ERRLOG("[!] Error: Failed to create compute kernel (%d)\n", err);
	}
#endif /* NYX_USE_OPENCL */
}

img_format_t::~img_format_t(void)
{
#ifdef NYX_USE_FFMPEG
	avpicture_free(&_picture);
	sws_freeContext(_sws_ctx);
#endif /* NYX_USE_FFMPEG */

#ifdef NYX_USE_OPENCL
	if (_cl_kernel)
		clReleaseKernel(_cl_kernel);
	if (_cl_program)
		clReleaseProgram(_cl_program);
#endif /* NYX_USE_OPENCL */
}

uint8_t* img_format_t::yuv420p_to_rgba(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height)
{
#ifdef NYX_USE_FFMPEG
	uint8_t* rgba = _yuv420p_to_rgba_ffmpeg(y, stride_y, u, stride_u, v, stride_v, width, height);
#else
#ifdef NYX_USE_OPENCL
	uint8_t* rgba = _yuv420p_to_rgba_cl(y, stride_y, u, stride_u, v, stride_v, width, height);
#else
	uint8_t* rgba = _yuv420p_to_rgba(y, u, v, width, height, stride_y);
#endif /* NYX_USE_OPENCL */
#endif /* NYX_USE_FFMPEG*/
	//write_tga("/Users/nyxouf/Desktop/_bla.tga", rgba, width, height);
	return rgba;
}

uint8_t* img_format_t::_yuv420p_to_rgba(const uint8_t* y, const uint8_t* u, const uint8_t* v, const size_t width, const size_t height, const size_t stride_y)
{
#ifdef NYX_BENCH
	clock_t start = clock();
#endif /* NYX_BENCH */

	const size_t size = width * height;
	uint8_t* rgba = (uint8_t*)calloc((size * 4), sizeof(uint8_t));

	uint8_t* ptr = rgba;
	int yy, uu, vv, r, g, b;
	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			yy = y[(j * stride_y) + i];
			uu = u[((j / 2) * (stride_y / 2)) + (i / 2)];
			vv = v[((j / 2) * (stride_y / 2)) + (i / 2)];

			r = 1.164 * (yy - 16) + 1.596 * (vv - 128);
			g = 1.164 * (yy - 16) - 0.813 * (vv - 128) - 0.391 * (uu - 128);
			b = 1.164 * (yy - 16) + 2.018 * (uu - 128);

			*ptr++ = NYX_SAFE_PIXEL_COMPONENT_VALUE(r);
			*ptr++ = NYX_SAFE_PIXEL_COMPONENT_VALUE(g);
			*ptr++ = NYX_SAFE_PIXEL_COMPONENT_VALUE(b);
			*ptr++ = NYX_MAX_PIXEL_COMPONENT_VALUE;
		}
	}

#ifdef NYX_BENCH
	clock_t end = clock();
	NYX_DLOG("\n[-] %s -> %fs\n", __func__, (double)(end - start) / CLOCKS_PER_SEC);
#endif /* NYX_BENCH */

	return rgba;
}

#ifdef NYX_USE_OPENCL
uint8_t* img_format_t::_yuv420p_to_rgba_cl(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height)
{
#ifdef NYX_BENCH
	clock_t start = clock();
#endif /* NYX_BENCH */

	cl_int err;
	cl_mem input_y = NULL, input_u = NULL, input_v = NULL; // device memory used for the input array
	cl_mem output = NULL; // device memory used for the output array
	const size_t origin[3] = {0};
	const size_t region_out[3] = {width, height, 1};
	const size_t gsize[2] = {stride_y, height};
	const size_t size = width * height;
	uint8_t* rgba = (uint8_t*)calloc((size * 4), sizeof(uint8_t));

	// create the input and output arrays in device memory for our calculation
	input_y = clCreateBuffer(_cl_context, CL_MEM_READ_ONLY, (height * stride_y), NULL, NULL);
	input_u = clCreateBuffer(_cl_context, CL_MEM_READ_ONLY, (height / 2 * stride_u), NULL, NULL);
	input_v = clCreateBuffer(_cl_context, CL_MEM_READ_ONLY, (height / 2 * stride_v), NULL, NULL);

	// create the input and output arrays in device memory for our calculation
#ifdef NYX_CL_USE_FLOAT_BUFFERS
	static const cl_image_format format = {CL_RGBA, CL_UNORM_INT8};
#else
	static const cl_image_format format = {CL_RGBA, CL_UNSIGNED_INT8};
#endif /* NYX_CL_USE_FLOAT_BUFFERS */
	cl_image_desc desc_out;
	desc_out.image_type = CL_MEM_OBJECT_IMAGE2D;
	desc_out.image_width = width;
	desc_out.image_height = height;
	desc_out.image_depth = 1;
	desc_out.image_array_size = 1;
	desc_out.image_row_pitch = 0;
	desc_out.image_slice_pitch = 0;
	desc_out.num_mip_levels = 0;
	desc_out.num_samples = 0;
	desc_out.buffer = NULL;
	output = clCreateImage(_cl_context, CL_MEM_WRITE_ONLY, &format, &desc_out, NULL, &err);
	if ((!input_y) || (!input_u) || (!input_v) || (!output))
	{
		NYX_ERRLOG("[!] Error: Failed to allocate device memory (%d) %p %p %p %p\n", err, input_y, input_u, input_v, output);
		goto out;
	}

	// write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(_cl_commands, input_y, CL_TRUE, 0, (height * stride_y), y, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to write to source array (%d)\n", err);
		goto out;
	}
	err = clEnqueueWriteBuffer(_cl_commands, input_u, CL_TRUE, 0, (height / 2 * stride_u), u, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to write to source array (%d)\n", err);
		goto out;
	}
	err = clEnqueueWriteBuffer(_cl_commands, input_v, CL_TRUE, 0, (height / 2 * stride_v), v, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to write to source array (%d)\n", err);
		goto out;
	}

	// set the arguments to our compute kernel
	err = 0;
	err =  clSetKernelArg(_cl_kernel, 0, sizeof(cl_mem), &input_y);
	err |= clSetKernelArg(_cl_kernel, 1, sizeof(size_t), &stride_y);
	err |= clSetKernelArg(_cl_kernel, 2, sizeof(cl_mem), &input_u);
	err |= clSetKernelArg(_cl_kernel, 3, sizeof(size_t), &stride_u);
	err |= clSetKernelArg(_cl_kernel, 4, sizeof(cl_mem), &input_v);
	err |= clSetKernelArg(_cl_kernel, 5, sizeof(size_t), &stride_v);
	err |= clSetKernelArg(_cl_kernel, 6, sizeof(size_t), &width);
	err |= clSetKernelArg(_cl_kernel, 7, sizeof(size_t), &height);
	err |= clSetKernelArg(_cl_kernel, 8, sizeof(cl_mem), &output);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to set kernel arguments (%d)\n", err);
		goto out;
	}

	// execute kernel
	err = clEnqueueNDRangeKernel(_cl_commands, _cl_kernel, 2, NULL, gsize, NULL, 0, NULL, NULL);
	if (err)
	{
		NYX_ERRLOG("[!] Error: Failed to execute kernel (%d)\n", err);
		goto out;
	}

	// wait for the command commands to get serviced before reading back results
	clFinish(_cl_commands);

	// read back the results from the device
	err = clEnqueueReadImage(_cl_commands, output, CL_TRUE, origin, region_out, 0, 0, rgba, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to read output array (%d)\n", err);
	}

	out:
		// Cleanup
		if (input_y) clReleaseMemObject(input_y);
		if (input_u) clReleaseMemObject(input_u);
		if (input_v) clReleaseMemObject(input_v);
		if (output) clReleaseMemObject(output);

#ifdef NYX_BENCH
	clock_t end = clock();
	NYX_DLOG("\n[-] %s -> %fs\n", __func__, (double)(end - start) / CLOCKS_PER_SEC);
#endif /* NYX_BENCH */

	return rgba;
}
#endif /* NYX_USE_OPENCL */

#ifdef NYX_USE_FFMPEG
uint8_t* img_format_t::_yuv420p_to_rgba_ffmpeg(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height)
{
#ifdef NYX_BENCH
	clock_t start = clock();
#endif /* NYX_BENCH */

	const uint8_t* data[3] = {y, u, v};
	const int linesize[3] = {(int)stride_y, (int)stride_u, (int)stride_v};
	sws_scale(_sws_ctx, data, linesize, 0, height, _picture.data, _picture.linesize);
	uint8_t* rgba = (uint8_t*)malloc(width * height * 4);
	memcpy(rgba, _picture.data[0], (width * height * 4));

#ifdef NYX_BENCH
	clock_t end = clock();
	NYX_DLOG("\n[-] %s -> %fs\n", __func__, (double)(end - start) / CLOCKS_PER_SEC);
#endif /* NYX_BENCH */

	return rgba;
}
#endif /* NYX_USE_FFMPEG */

bool img_format_t::write_tga(const char* filepath, const uint8_t* buf, const size_t width, const size_t height)
{
	if (!buf)
		return false;

	bool ret = false;
	FILE* fp = fopen(filepath, "wb");
	if (!fp)
		return ret;

	// TGA Header
	uint8_t header[18] = {0};
	header[2] = 2; // RGB
	header[12] = width & 0xFF;
	header[13] = (width >> 8) & 0xFF;
	header[14] = height & 0xFF;
	header[15] = (height >> 8) & 0xFF;
	header[16] = 24; // bits per pixel
	fwrite(header, sizeof(uint8_t), 18, fp);

	const rgba_pixel_t* pixels = (rgba_pixel_t*)buf;
	for (int y = (int)height - 1; y >= 0; y--)
	{
		for (size_t x = 0; x < width; x++)
		{
			const rgba_pixel_t pixel = pixels[((size_t)y * width) + x];
			fwrite((uint8_t[3]){pixel.b, pixel.g, pixel.r}, sizeof(uint8_t), 3, fp);
		}
	}
	ret = true;

	// cleanup
	fclose(fp);

	return ret;
}

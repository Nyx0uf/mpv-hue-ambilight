#include "cl_global.h"
#include "global.h"
#include <cstdio>


static cl_device_id __device_id = NULL;
static cl_context __context = NULL;
static cl_command_queue __commands = NULL;
static cl_uint __vector_width[2] = {0};
static bool __is_init = false;


bool nyx_cl_init(void)
{
	if (__is_init)
		return true;

	cl_int err = CL_SUCCESS;
#ifdef DEBUG
	cl_platform_id platforms[10] = {NULL};
	cl_uint num_platforms = 0;
	err = clGetPlatformIDs(10, platforms, &num_platforms);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to get platforms (%d)\n", err);
		return false;
	}

	NYX_DLOG("%d OpenCL platform%s found:\n", num_platforms, (num_platforms > 1) ? "s" : "");
	for (cl_uint i = 0; i < num_platforms; i++)
	{
		char buffer[1024];
		NYX_DLOG("-- platform %d --\n", (i + 1));
		clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, 1024, buffer, NULL);
		NYX_DLOG(" PROFILE = %s\n", buffer);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 1024, buffer, NULL);
		NYX_DLOG(" VERSION = %s\n", buffer);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, buffer, NULL);
		NYX_DLOG(" NAME = %s\n", buffer);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 1024, buffer, NULL);
		NYX_DLOG(" VENDOR = %s\n", buffer);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 1024, buffer, NULL);
		NYX_DLOG(" EXTENSIONS = %s\n", buffer);
	}
#endif /* DEBUG */

	// connect to a compute device
	err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &__device_id, NULL);
	if (err != CL_SUCCESS)
	{
		NYX_ERRLOG("[!] Error: Failed to create a device group for type <CL_DEVICE_TYPE_GPU> (%d)\n", err);
		return false;
	}

	// create a compute context
	__context = clCreateContext(0, 1, &__device_id, NULL, NULL, &err);
	if (!__context)
	{
		NYX_ERRLOG("[!] Error: Failed to create a compute context (%d)\n", err);
		__device_id = NULL;
		return false;
	}

	// create a command commands
	__commands = clCreateCommandQueue(__context, __device_id, 0, &err);
	if (!__commands)
	{
		NYX_ERRLOG("[!] Error: Failed to create a command queue (%d)\n", err);
		clReleaseContext(__context), __context = NULL;
		__device_id = NULL;
		return false;
	}

	// get some global params
	clGetDeviceInfo(__device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &(__vector_width[0]), NULL);
	clGetDeviceInfo(__device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &(__vector_width[1]), NULL);
	NYX_DLOG("[+] CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT : %d\n[+] CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT : %d\n", __vector_width[0], __vector_width[1]);

	__is_init = true;
	return true;
}

void nyx_cl_uninit(void)
{
	if (__is_init)
	{
		clReleaseCommandQueue(__commands), __commands = NULL;
		clReleaseContext(__context), __context = NULL;
		__device_id = NULL;
		__is_init = false;
	}
}

cl_device_id nyx_cl_get_deviceid(void)
{
	return __device_id;
}

cl_context nyx_cl_get_context(void)
{
	return __context;
}

cl_command_queue nyx_cl_get_commandqueue(void)
{
	return __commands;
}

cl_uint nyx_cl_get_int_vector_width(void)
{
	return __vector_width[0];
}

cl_uint nyx_cl_get_float_vector_width(void)
{
	return __vector_width[1];
}

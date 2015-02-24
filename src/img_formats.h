#ifndef __NYX_IMG_FORMATS_H__
#define __NYX_IMG_FORMATS_H__


#include <cstdlib>
#include "global.h"

#ifdef NYX_USE_OPENCL
#include "cl_global.h"
#endif /* NYX_USE_OPENCL */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef NYX_USE_FFMPEG
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#endif /* NYX_USE_FFMPEG */

#ifdef __cplusplus
}
#endif /* __cplusplus */


class img_format_t
{
private:
#ifdef NYX_USE_FFMPEG
	/// FFMPEG sws context
	struct SwsContext* _sws_ctx;
	/// FFMPEG picture
	AVPicture _picture;
#endif /* NYX_USE_FFMPEG */
#ifdef NYX_USE_OPENCL
	/// OpenCL device ID
	cl_device_id _cl_device_id;
	/// OpenCL context
	cl_context _cl_context;
	/// OpenCL commands queue
	cl_command_queue _cl_commands;
	/// OpenCL program
	cl_program _cl_program;
	/// OpenCL kernel
	cl_kernel _cl_kernel;
#endif /* NYX_USE_OPENCL */

public:
	/**
	 * @brief Initialize OpenCL and FFMPEG stuff
	 * @param width [in] : image width
	 * @param height [in] : image height
	 */
	img_format_t(const size_t width, const size_t height);

	/**
	 * @brief Destructor, dealloc OpenCL and FFMPEG stuff
	 */
	~img_format_t(void);

	/**
	 * @brief Convert YUV420P buffers to a single RGBA buffer
	 * @param y [in] : Y plane
	 * @param stride_y [in] : stride of the Y plane
	 * @param u [in] : U plane
	 * @param stride_u [in] : stride of the U plane
	 * @param v [in] : V plane
	 * @param stride_v [in] : stride of the V plane
	 * @param width [in] : image width
	 * @param height [in] : image height
	 * @returns rgba buffer, or NULL if failure
	 */
	uint8_t* yuv420p_to_rgba(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height);

private:
	/**
	 * @brief Convert YUV420P buffers to a single RGBA buffer, naive method
	 * @param y [in] : Y plane
	 * @param u [in] : U plane
	 * @param v [in] : V plane
	 * @param width [in] : image width
	 * @param height [in] : image height
	 * @param stride_y [in] : stride of the Y plane
	 * @returns rgba buffer, or NULL if failure
	 */
	uint8_t* _yuv420p_to_rgba(const uint8_t* y, const uint8_t* u, const uint8_t* v, const size_t width, const size_t height, const size_t stride_y);

#ifdef NYX_USE_OPENCL
	/**
	 * @brief Convert YUV420P buffers to a single RGBA buffer, using OpenCL
	 * @param y [in] : Y plane
	 * @param stride_y [in] : stride of the Y plane
	 * @param u [in] : U plane
	 * @param stride_u [in] : stride of the U plane
	 * @param v [in] : V plane
	 * @param stride_v [in] : stride of the V plane
	 * @param width [in] : image width
	 * @param height [in] : image height
	 * @returns rgba buffer, or NULL if failure
	 */
	uint8_t* _yuv420p_to_rgba_cl(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height);
#endif /* NYX_USE_OPENCL */

#ifdef NYX_USE_FFMPEG
	/**
	 * @brief Convert YUV420P buffers to a single RGBA buffer, using FFMPEG
	 * @param y [in] : Y plane
	 * @param stride_y [in] : stride of the Y plane
	 * @param u [in] : U plane
	 * @param stride_u [in] : stride of the U plane
	 * @param v [in] : V plane
	 * @param stride_v [in] : stride of the V plane
	 * @param width [in] : image width
	 * @param height [in] : image height
	 * @returns rgba buffer, or NULL if failure
	 */
	uint8_t* _yuv420p_to_rgba_ffmpeg(const uint8_t* y, const size_t stride_y, const uint8_t* u, const size_t stride_u, const uint8_t* v, const size_t stride_v, const size_t width, const size_t height);
#endif /* NYX_USE_FFMPEG */

public:
	/**
	 * @brief Write a RGBA buffer to a TGA file
	 * @param filepath [in] : path where to save the file
	 * @param buf [in] : RGBA buffer
	 * @param width [in] : image width
	 * @param height [in] : image height
	 * @returns true if all OK.
	 */
	static bool write_tga(const char* filepath, const uint8_t* buf, const size_t width, const size_t height);
};

#endif /* __NYX_IMG_FORMATS_H__ */

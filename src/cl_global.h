#ifndef __NYX_CL_GLOBAL_H__
#define __NYX_CL_GLOBAL_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif /* __APPLE__ */

/**
 * @brief Init OpenCL by creating a device, context and command queue
 * @returns true if all was OK
 */
bool nyx_cl_init(void);

/**
 * @brief Cleanup OpenCL stuff
 */
void nyx_cl_uninit(void);

/**
 * @brief get the OpenCL device
 * @returns OpenCL device if it was init, NULL otherwise
 */
cl_device_id nyx_cl_get_deviceid(void);

/**
 * @brief get the OpenCL context
 * @returns OpenCL context if it was init, NULL otherwise
 */
cl_context nyx_cl_get_context(void);

/**
 * @brief get the OpenCL command queue
 * @returns OpenCL command queue if it was init, NULL otherwise
 */
cl_command_queue nyx_cl_get_commandqueue(void);

/**
 * @brief get the best vector width for integer operations
 * @returns vector width
 */
cl_uint nyx_cl_get_int_vector_width(void);

/**
 * @brief get the best vector width for float operations
 * @returns vector width
 */
cl_uint nyx_cl_get_float_vector_width(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NYX_CL_GLOBAL_H__ */

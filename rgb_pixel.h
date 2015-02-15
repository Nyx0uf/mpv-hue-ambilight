#ifndef __NYX_RGB_PIXEL_H__
#define __NYX_RGB_PIXEL_H__


#include <functional>
#include "global.h"


/* Minimun value for a RGB pixel component */
#define NYX_MIN_PIXEL_COMPONENT_VALUE ((uint8_t)0)
/* Maximum value for a RGB pixel component */
#define NYX_MAX_PIXEL_COMPONENT_VALUE ((uint8_t)UINT8_MAX)
/* Returns a correct value for a RGB pixel component (0 - 255) */
#define NYX_SAFE_PIXEL_COMPONENT_VALUE(COLOR) (NYX_CLAMP(COLOR, NYX_MIN_PIXEL_COMPONENT_VALUE, NYX_MAX_PIXEL_COMPONENT_VALUE))


struct rgb_pixel_t
{
	/// Red
	uint8_t r;
	/// Green
	uint8_t g;
	/// Blue
	uint8_t b;

	/// Default constructor (black pixel)
	rgb_pixel_t(void) {r = NYX_MIN_PIXEL_COMPONENT_VALUE; g = NYX_MIN_PIXEL_COMPONENT_VALUE; b = NYX_MIN_PIXEL_COMPONENT_VALUE;}
	/// Constructor
	rgb_pixel_t(const uint8_t red, const uint8_t green, const uint8_t blue) {r = red; g = green; b = blue;}

	inline bool is_black(void)const {return (r == NYX_MIN_PIXEL_COMPONENT_VALUE && g == NYX_MIN_PIXEL_COMPONENT_VALUE && b == NYX_MIN_PIXEL_COMPONENT_VALUE);}
	inline bool is_white(void)const {return (r == NYX_MAX_PIXEL_COMPONENT_VALUE && g == NYX_MAX_PIXEL_COMPONENT_VALUE && b == NYX_MAX_PIXEL_COMPONENT_VALUE);}
	bool is_dark(void)const;

	// Operators overloading
	inline bool operator==(const rgb_pixel_t& px)const
	{
		return ((r == px.r) && (g == px.g) && (b == px.b));
	}
};

namespace std
{
	template <>
	struct hash<rgb_pixel_t>
	{
		inline size_t operator()(const rgb_pixel_t& px)const
		{
			return (uint64_t(px.r << 16) | uint64_t(px.g << 8) | uint64_t(px.b));
		}
	};
}

#endif /* __NYX_RGB_PIXEL_H__ */

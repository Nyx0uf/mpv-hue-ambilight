#ifndef __NYX_RGBA_PIXEL_H__
#define __NYX_RGBA_PIXEL_H__


#include <functional>
#include "global.h"


/* Minimun value for a RGB pixel component */
#define NYX_MIN_PIXEL_COMPONENT_VALUE ((uint8_t)0)
/* Maximum value for a RGB pixel component */
#define NYX_MAX_PIXEL_COMPONENT_VALUE ((uint8_t)UINT8_MAX)
/* Returns a correct value for a RGB pixel component (0 - 255) */
#define NYX_SAFE_PIXEL_COMPONENT_VALUE(COLOR) (NYX_CLAMP(COLOR, NYX_MIN_PIXEL_COMPONENT_VALUE, NYX_MAX_PIXEL_COMPONENT_VALUE))


struct rgba_pixel_t
{
	/// Red component
	uint8_t r;
	/// Green component
	uint8_t g;
	/// Blue component
	uint8_t b;
	/// Alpha component
	uint8_t a;

	/**
	 * @brief Default constructor: Initialize a black pixel
	 */
	rgba_pixel_t(void);

	/**
	 * @brief Initialize a pixel
	 * @param red [in] : red component
	 * @param green [in] : green component
	 * @param blue [in] : blue component
	 * @param alpha [in] : alpha component
	 */
	rgba_pixel_t(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha = NYX_MAX_PIXEL_COMPONENT_VALUE);

	/**
	 * @brief Create a copy of another pixel
	 * @param px [in] : Pixel to copy
	 */
	rgba_pixel_t(const rgba_pixel_t& px);

	/**
	 * @brief Check if a pixel is black ignoring alpha
	 * @returns true if the pixel is black
	 */
	inline bool is_black(void)const {return (r == NYX_MIN_PIXEL_COMPONENT_VALUE && g == NYX_MIN_PIXEL_COMPONENT_VALUE && b == NYX_MIN_PIXEL_COMPONENT_VALUE);}

	/**
	 * @brief Check if a pixel is white ignoring alpha
	 * @returns true if the pixel is white
	 */
	inline bool is_white(void)const {return (r == NYX_MAX_PIXEL_COMPONENT_VALUE && g == NYX_MAX_PIXEL_COMPONENT_VALUE && b == NYX_MAX_PIXEL_COMPONENT_VALUE);}

	/**
	 * @brief Check if a pixel is transparent
	 * @returns true if the pixel is transparent
	 */
	inline bool is_transparent(void)const {return (a == NYX_MIN_PIXEL_COMPONENT_VALUE);}

	/**
	 * @brief Check if a pixel is dark
	 * @returns true if the pixel is dark
	 */
	bool is_dark(void)const;

	/**
	 * Operators overloading
	 */
	inline bool operator==(const rgba_pixel_t& px)const
	{
		return ((r == px.r) && (g == px.g) && (b == px.b) && (a == px.a));
	}
};

/**
 * std::hash overloading
 */
namespace std
{
	template <>
	struct hash<rgba_pixel_t>
	{
		inline size_t operator()(const rgba_pixel_t& px)const
		{
			return size_t((px.a << 24) + (px.b << 16) + (px.g << 8) + px.r);
		}
	};
}


/**
 * Normalized pixel component values, uint8_t -> double (0.0 -> 1.0)
 */
extern double __normalized_component_values[256];

#endif /* __NYX_RGBA_PIXEL_H__ */

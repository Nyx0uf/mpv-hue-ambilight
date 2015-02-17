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
	/// Red component
	uint8_t r;
	/// Green component
	uint8_t g;
	/// Blue component
	uint8_t b;

	/**
	 * @brief Default constructor: Initialize a black pixel
	 */
	rgb_pixel_t(void);

	/**
	 * @brief Initialize a pixel
	 * @param red [in] : red component
	 * @param green [in] : green component
	 * @param blue [in] : blue component
	 */
	rgb_pixel_t(const uint8_t red, const uint8_t green, const uint8_t blue);

	/**
	 * @brief Create a copy of another pixel
	 * @param px [in] : Pixel to copy
	 */
	rgb_pixel_t(const rgb_pixel_t& px);

	/**
	 * @brief Check if a pixel is black
	 * @returns true if the pixel is black
	 */
	inline bool is_black(void)const {return (r == NYX_MIN_PIXEL_COMPONENT_VALUE && g == NYX_MIN_PIXEL_COMPONENT_VALUE && b == NYX_MIN_PIXEL_COMPONENT_VALUE);}

	/**
	 * @brief Check if a pixel is white
	 * @returns true if the pixel is white
	 */
	inline bool is_white(void)const {return (r == NYX_MAX_PIXEL_COMPONENT_VALUE && g == NYX_MAX_PIXEL_COMPONENT_VALUE && b == NYX_MAX_PIXEL_COMPONENT_VALUE);}

	/**
	 * @brief Check if a pixel is dark
	 * @returns true if the pixel is dark
	 */
	bool is_dark(void)const;

	/**
	 * Operators overloading
	 */
	inline bool operator==(const rgb_pixel_t& px)const
	{
		return ((r == px.r) && (g == px.g) && (b == px.b));
	}
};

/**
 * std::hash overloading
 */
namespace std
{
	template <>
	struct hash<rgb_pixel_t>
	{
		inline size_t operator()(const rgb_pixel_t& px)const
		{
			return (size_t(px.r << 16) | size_t(px.g << 8) | size_t(px.b));
		}
	};
}


/**
 * Normalized pixel component values, uint8_t -> double (0.0 -> 1.0)
 */
extern double __normalized_component_values[256];

#endif /* __NYX_RGB_PIXEL_H__ */

#include "rgb_pixel.h"


bool rgb_pixel_t::is_dark(void)const
{
	const double lum = 0.2126 * (r/255.0) + 0.7152 * (g/255.0) + 0.0722 * (b/255.0);
	return (lum < .5);
}
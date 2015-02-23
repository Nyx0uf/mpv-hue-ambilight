#include "hue_controller.h"
#include "global.h"
#include "rgba_pixel.h"
#include "point.h"
#include "hue_client.h"
#include <cmath>
#include <unordered_set>
#include <vector>
#include <utility>


#define COLOR_THRESHOLD_MIN_PERCENTAGE 0.005
#define DEFAULT_SATURATION_VALUE 255
#define DEFAULT_BRIGHTNESS_VALUE 32
#define DEFAULT_TRANSITION_TIME 5


typedef std::pair<rgba_pixel_t, int> counted_pixel_t;
typedef std::vector<counted_pixel_t> counted_pixel_vector_t;


hue_controller_t::hue_controller_t(const char* hue_ip, const uint8_t n_lamp)
{
	const size_t len = strlen(hue_ip);
	this->_hue_ip = (char*)calloc(len + 1, sizeof(char));
	memcpy(this->_hue_ip, hue_ip, len);
	this->_number_lamps = n_lamp;
}

hue_controller_t::~hue_controller_t(void)
{
	free(this->_hue_ip);
}

void hue_controller_t::apply_dominant_color_from_buffer(const uint8_t* buffer, const size_t width, const size_t height)const
{
	rgba_pixel_t* pixels = (rgba_pixel_t*)buffer;
	// left edge (8px range)
	std::unordered_multiset<rgba_pixel_t> left_edge_colors;
	this->get_left_edge(pixels, width, height, left_edge_colors, 8);
	// right edge (8px range)
	std::unordered_multiset<rgba_pixel_t> right_edge_colors;
	this->get_right_edge(pixels, width, height, right_edge_colors, 8);

	// Assuming:
	// lamp 1 on the right
	// lamp 2 on the left
	// lamp 3 at the center
	const int random_colors_threshold = (int)(height * COLOR_THRESHOLD_MIN_PERCENTAGE);
	this->apply_color_to_lamp(left_edge_colors, random_colors_threshold, 2);
	this->apply_color_to_lamp(right_edge_colors, random_colors_threshold, 1);
}

void hue_controller_t::get_left_edge(rgba_pixel_t* pixels, const size_t width, const size_t height, std::unordered_multiset<rgba_pixel_t>& edge, const size_t col)const
{
	const size_t size = height * width;
	for (size_t y = 0; y < size; y += width)
	{
		for (size_t w = 0; w < col; ++w)
		{
			edge.insert(pixels[y + w]);
		}
	}
}

void hue_controller_t::get_right_edge(rgba_pixel_t* pixels, const size_t width, const size_t height, std::unordered_multiset<rgba_pixel_t>& edge, const size_t col)const
{
	const size_t size = height * width;
	for (size_t y = (width - 1); y < size; y += width)
	{
		for (size_t w = 0; w < col; ++w)
		{
			edge.insert(pixels[y - w]);
		}
	}
}

void hue_controller_t::apply_color_to_lamp(std::unordered_multiset<rgba_pixel_t>& edge, const int random_colors_threshold, const int lamp_n)const
{
	counted_pixel_vector_t colors;
	for (auto cur_color : edge)
	{
		const int color_count = (int)edge.count(cur_color);
		if (color_count <= random_colors_threshold) // prevent using random colors
			continue;

		colors.emplace_back(cur_color, color_count);
	}

	// Got some colors
	if (colors.size() > 0)
	{
		hue_client_t hue(this->_hue_ip);
		if (!hue.connect())
		{
			return;
		}

		std::sort(colors.begin(), colors.end(), [&](const counted_pixel_vector_t::value_type& i1, const counted_pixel_vector_t::value_type& i2) {
			return (i1.second > i2.second);
		});
		counted_pixel_t cpx = colors[0];
		rgba_pixel_t px = cpx.first;
		//NYX_DLOG("DARK : %d\n", px.is_dark());

		char json[512] = {0x00};
		if (px.is_black())
		{
			NYX_DLOG("[+] Dark color.\n");

			sprintf(json, "{\"on\":true, \"transitiontime\":%d, \"sat\":%d, \"bri\":%d, \"xy\":[0.35,0.35]}", DEFAULT_TRANSITION_TIME, DEFAULT_SATURATION_VALUE, DEFAULT_BRIGHTNESS_VALUE); // Kinda white
			hue.send_command(json, lamp_n);
		}
		else
		{
			const point_t p = calculate_XY_from_RGB_and_model(&px, HUE_LAMP_MODEL::LCT001);
			NYX_DLOG("r=%d g=%d b=%d (%d) | x=%f y=%f\n", px.r, px.g, px.b, cpx.second, p.x, p.y);

			sprintf(json, "{\"on\":true, \"transitiontime\":%d, \"sat\":%d, \"bri\":%d, \"xy\":[%f,%f]}", DEFAULT_TRANSITION_TIME, DEFAULT_SATURATION_VALUE, DEFAULT_BRIGHTNESS_VALUE, p.x, p.y);
			hue.send_command(json, lamp_n);
		}
	}
	else
	{
		NYX_DLOG("[!] Color matching failed.\n");
	}
}

point_t hue_controller_t::calculate_XY_from_RGB_and_model(const rgba_pixel_t* px, const HUE_LAMP_MODEL model)
{
	// Normalize
	const double red = __normalized_component_values[px->r];
	const double green = __normalized_component_values[px->g];
	const double blue = __normalized_component_values[px->b];
	
	// Apply gamma correction
	const double r = (red > 0.04045) ? std::pow((red + 0.055) / (1.0 + 0.055), 2.4) : (red / 12.92);
	const double g = (green > 0.04045) ? std::pow((green + 0.055) / (1.0 + 0.055), 2.4) : (green / 12.92);
	const double b = (blue > 0.04045) ? std::pow((blue + 0.055) / (1.0 + 0.055), 2.4) : (blue / 12.92);

	// Wide gamut conversion D65
	const double x = r * 0.649926 + g * 0.103455 + b * 0.197109;
	const double y = r * 0.234327 + g * 0.743075 + b * 0.022598;
	const double z = r * 0.0000000 + g * 0.053077 + b * 1.035763;

	const double xyz = (x + y + z);
	double cx = x / xyz;
	double cy = y / xyz;

	if (NYX_IS_NAN(cx))
		cx = 0.0;

	if (NYX_IS_NAN(cy))
		cy = 0.0;

	// Check if the given XY value is within the colourreach of our lamps.
	const point_t xy_point = point_t(cx, cy);
	point_t* color_points = color_points_for_model(model);
	const bool in_reach_of_lamps = check_point_in_lamps_reach_with_color_points(xy_point, color_points);

	if (!in_reach_of_lamps)
	{
		// It seems the colour is out of reach
		// let's find the closest colour we can produce with our lamp and send this XY value out.
		// Find the closest point on each line in the triangle.
		const point_t p_ab = get_closest_point_to_points(color_points[0], color_points[1], xy_point);
		const point_t p_ac = get_closest_point_to_points(color_points[2], color_points[0], xy_point);
		const point_t p_bc = get_closest_point_to_points(color_points[1], color_points[2], xy_point);

		// Get the distances per point and see which point is closer to our Point.
		const double d_ab = point_t::distance_between_points(xy_point, p_ab);
		const double d_ac = point_t::distance_between_points(xy_point, p_ac);
		const double d_bc = point_t::distance_between_points(xy_point, p_bc);

		double lowest = d_ab;
		point_t closest_point = p_ab;

		if (d_ac < lowest)
		{
			lowest = d_ac;
			closest_point = p_ac;
		}
		if (d_bc < lowest)
		{
			lowest = d_bc;
			closest_point = p_bc;
		}

		// Change the xy value to a value which is within the reach of the lamp.
		free(color_points);
		return closest_point;
	}
	else
	{
		free(color_points);
		return xy_point;
	}
}

point_t* hue_controller_t::color_points_for_model(const HUE_LAMP_MODEL model)
{
	point_t* color_points = (point_t*)calloc(3, sizeof(point_t));
	if (HUE_LAMP_MODEL::LCT001 == model || HUE_LAMP_MODEL::LCT002 == model || HUE_LAMP_MODEL::LCT003 == model)
	{
		// Hue bulbs color gamut triangle
		color_points[0] = point_t(0.674, 0.322); // RED
		color_points[1] = point_t(0.408, 0.517); // GREEN
		color_points[2] = point_t(0.168, 0.041); // BLUE
	}
	else if (HUE_LAMP_MODEL::LLC001 == model || HUE_LAMP_MODEL::LLC005 == model || HUE_LAMP_MODEL::LLC006 == model || HUE_LAMP_MODEL::LLC007 == model || HUE_LAMP_MODEL::LLC011 == model || HUE_LAMP_MODEL::LLC012 == model || HUE_LAMP_MODEL::LLC013 == model || HUE_LAMP_MODEL::LST001 == model)
	{
		// LivingColors color gamut triangle
		color_points[0] = point_t(0.703, 0.296); // RED
		color_points[1] = point_t(0.214, 0.709); // GREEN
		color_points[2] = point_t(0.139, 0.081); // BLUE
	}
	else
	{
		// Default construct triangle wich contains all values
		color_points[0] = point_t(1.0, 0.0); // RED
		color_points[1] = point_t(0.0, 1.0); // GREEN
		color_points[2] = point_t(0.0, 0.0); // BLUE
	}

	return color_points;
}

/**
 * Find the closest point on a line.
 * This point will be within reach of the lamp.
 *
 * @param pta the point where the line starts
 * @param ptb the point where the line ends
 * @param ptp the point which is close to a line.
 * @return the point which is on the line.
 */
point_t hue_controller_t::get_closest_point_to_points(const point_t& pta, const point_t& ptb, const point_t& ptp)
{
	const point_t p_ap = point_t(ptp.x - pta.x, ptp.y - pta.y);
	const point_t p_ab = point_t(ptb.x - pta.x, ptb.y - pta.y);
	const double ab2 = p_ab.x * p_ab.x + p_ab.y * p_ab.y;
	const double ap_ab = p_ap.x * p_ab.x + p_ap.y * p_ab.y;

	double t = ap_ab / ab2;

	if (t < 0.0)
		t = 0.0;
	else if (t > 1.0)
		t = 1.0;

	return point_t(pta.x + p_ab.x * t, pta.y + p_ab.y * t);
}

/**
 * Method to see if the given XY value is within the reach of the lamps.
 *
 * @param pt the point containing the X,Y value
 * @return true if within reach, false otherwise.
 */
bool hue_controller_t::check_point_in_lamps_reach_with_color_points(const point_t& pt, const point_t* color_points)
{
	const point_t red = color_points[0];
	const point_t green = color_points[1];
	const point_t blue = color_points[2];

	const point_t v1 = point_t(green.x - red.x, green.y - red.y);
	const point_t v2 = point_t(blue.x - red.x, blue.y - red.y);

	const point_t q = point_t(pt.x - red.x, pt.y - red.y);

	// cross product (pt1.x * pt2.y - pt1.y * pt2.x)
	const double s = (q.x * v2.y - q.y * v2.x);
	const double t = (v1.x * q.y - v1.y * q.x);

	if ((s >= 0.0) && (t >= 0.0) && (s + t <= 1.0))
		return true;
	else
		return false;
}

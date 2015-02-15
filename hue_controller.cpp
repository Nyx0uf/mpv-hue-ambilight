#include "hue_controller.h"
#include "global.h"
#include "rgb_pixel.h"
#include "point.h"
#include "hue_client.h"
#include <unordered_set>
#include <cmath>
#include <vector>
#include <utility>


#define COLOR_THRESHOLD_MIN_PERCENTAGE 0.005

typedef std::pair<rgb_pixel_t, int> counted_pixel_t;
typedef std::vector<counted_pixel_t> counted_pixel_vector_t;


/* Constructor */
hue_controller_t::hue_controller_t(const char* hue_ip)
{
	const size_t len = strlen(hue_ip);
	this->_hue_ip = (char*)calloc(len + 1, sizeof(char));
	memcpy(this->_hue_ip, hue_ip, len);
}

/* Destructor */
hue_controller_t::~hue_controller_t(void)
{
	free(this->_hue_ip);
}

void hue_controller_t::apply_dominant_color_from_buffer(const uint8_t* buffer, const size_t width, const size_t height)const
{
	const rgb_pixel_t* pixels = (rgb_pixel_t*)buffer;
	// left edge (8px range)
	const size_t size = height * width;
	std::unordered_multiset<rgb_pixel_t> left_edge_colors;
	for (size_t y = 0; y < size; y += width)
	{
		left_edge_colors.insert(pixels[y]);
		left_edge_colors.insert(pixels[y+1]);
		left_edge_colors.insert(pixels[y+2]);
		left_edge_colors.insert(pixels[y+3]);
		left_edge_colors.insert(pixels[y+4]);
		left_edge_colors.insert(pixels[y+5]);
		left_edge_colors.insert(pixels[y+6]);
		left_edge_colors.insert(pixels[y+7]);
	}

	const int random_colors_threshold = (int)(height * COLOR_THRESHOLD_MIN_PERCENTAGE);
	counted_pixel_vector_t colors;
	for (auto cur_color : left_edge_colors)
	{
		const int color_count = (int)left_edge_colors.count(cur_color);
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

		std::sort(colors.begin(), colors.end(), [&](const counted_pixel_vector_t::value_type &i1, const counted_pixel_vector_t::value_type &i2) {
			return i1.second > i2.second;
		});
		counted_pixel_t cpx = colors[0];
		rgb_pixel_t px = cpx.first;
		//NYX_DLOG("DARK : %d\n", px.is_dark());
		//rgb_pixel_t tmp = sortedc[sortedc.size()-1].first;
		//NYX_DLOG("TEST1: %d %d %d (%d)\n", px.r, px.g, px.b, sortedc[0].second);
		//NYX_DLOG("TEST2: %d %d %d (%d)\n", tmp.r, tmp.g, tmp.b, sortedc[sortedc.size()-1].second);

		if (px.is_black())
		{
			NYX_DLOG("[+] Black color.\n");

			char json[1024] = {0x00};
			sprintf(json, "{\"on\":true, \"transitiontime\":5, \"sat\":%d, \"bri\":%d, \"xy\":[0.35,0.35]}", 255, 32);
			hue.send_command(json);
		}
		else
		{
			const point_t p = calculateXYFromRGBandModel(px, "LCT001");
			NYX_DLOG("r=%d g=%d b=%d (%d) | x=%f y=%f\n", px.r, px.g, px.b, cpx.second, p.x, p.y);

			char json[1024] = {0x00};
			sprintf(json, "{\"on\":true, \"transitiontime\":5, \"sat\":%d, \"bri\":%d, \"xy\":[%f,%f]}", 255, 32, p.x, p.y);
			hue.send_command(json);
		}
	}
	else
	{
		NYX_DLOG("[!] Color matching failed.\n");
	}
}

/*** Static ***/
/*
Taken and adapted from: https://github.com/PhilipsHue/PhilipsHueSDK-iOS-OSX/blob/master/ApplicationDesignNotes/RGB%20to%20xy%20Color%20conversion.md
*/
point_t hue_controller_t::calculateXYFromRGBandModel(const rgb_pixel_t& px, const char* model)
{
	// Normalize
	const double red = (px.r / 255.0);
	const double green = (px.g / 255.0);
	const double blue = (px.b / 255.0);
	
	// Apply gamma correction
	const double r = (red > 0.04045) ? std::pow((red + 0.055) / (1.0 + 0.055), 2.4) : (red / 12.92);
	const double g = (green > 0.04045) ? std::pow((green + 0.055) / (1.0 + 0.055), 2.4) : (green / 12.92);
	const double b = (blue > 0.04045) ? std::pow((blue + 0.055) / (1.0 + 0.055), 2.4) : (blue / 12.92);

	// Wide gamut conversion D65
	const double X = r * 0.649926 + g * 0.103455 + b * 0.197109;
	const double Y = r * 0.234327 + g * 0.743075 + b * 0.022598;
	const double Z = r * 0.0000000 + g * 0.053077 + b * 1.035763;

	const double XYZ = (X + Y + Z);
	double cx = X / XYZ;
	double cy = Y / XYZ;

	if (isnan(cx))
		cx = 0.0;

	if (isnan(cy))
		cy = 0.0;

	//Check if the given XY value is within the colourreach of our lamps.
	const point_t xy_point = point_t(cx, cy);
	point_t* color_points = colorPointsForModel(model);
	const bool in_reach_of_lamps = checkPointInLampsReachwithColorPoints(xy_point, color_points);

	if (!in_reach_of_lamps)
	{
		//It seems the colour is out of reach
		//let's find the closest colour we can produce with our lamp and send this XY value out.

		//Find the closest point on each line in the triangle.
		const point_t p_ab = getClosestPointToPoints(color_points[0], color_points[1], xy_point);
		const point_t p_ac = getClosestPointToPoints(color_points[2], color_points[0], xy_point);
		const point_t p_bc = getClosestPointToPoints(color_points[1], color_points[2], xy_point);

		// Get the distances per point and see which point is closer to our Point.
		const double d_ab = getDistanceBetweenTwoPoints(xy_point, p_ab);
		const double d_ac = getDistanceBetweenTwoPoints(xy_point, p_ac);
		const double d_bc = getDistanceBetweenTwoPoints(xy_point, p_bc);

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

		//Change the xy value to a value which is within the reach of the lamp.
		cx = closest_point.x;
		cy = closest_point.y;
	}
	free(color_points);

	return point_t(cx, cy);
}

point_t* hue_controller_t::colorPointsForModel(const char* model)
{
	point_t* color_points = (point_t*)calloc(3, sizeof(point_t));
	if (strcmp(model, "LCT001") == 0 || strcmp(model, "LCT002") == 0 || strcmp(model, "LCT003") == 0)
	{
		// Hue bulbs color gamut triangle
		color_points[0] = point_t(0.674, 0.322); // RED
		color_points[1] = point_t(0.408, 0.517); // GREEN
		color_points[2] = point_t(0.168, 0.041); // BLUE
	}
	else if (strcmp(model, "LLC001") == 0 || strcmp(model, "LLC005") == 0 || strcmp(model, "LLC006") == 0 || strcmp(model, "LLC007") == 0 || strcmp(model, "LLC011") == 0 || strcmp(model, "LLC012") == 0 || strcmp(model, "LLC013") == 0 || strcmp(model, "LST001") == 0)
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
point_t hue_controller_t::getClosestPointToPoints(const point_t& pta, const point_t& ptb, const point_t& ptp)
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
 * Find the distance between two points.
 *
 * @param pt1
 * @param pt2
 * @return the distance between point one and two
 */
double hue_controller_t::getDistanceBetweenTwoPoints(const point_t& pt1, const point_t& pt2)
{
	const double dx = pt1.x - pt2.x; // horizontal difference
	const double dy = pt1.y - pt2.y; // vertical difference
	return sqrt(dx * dx + dy * dy);
}

/**
 * Method to see if the given XY value is within the reach of the lamps.
 *
 * @param pt the point containing the X,Y value
 * @return true if within reach, false otherwise.
 */
bool hue_controller_t::checkPointInLampsReachwithColorPoints(const point_t& pt, const point_t* color_points)
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

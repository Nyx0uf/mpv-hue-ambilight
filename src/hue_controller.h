#ifndef __NYX_HUE_CONTROLLER_H__
#define __NYX_HUE_CONTROLLER_H__


#include <unordered_set>
#include <cstdlib>
struct rgba_pixel_t;
struct point_t;


enum class HUE_LAMP_MODEL : int
{
	LCT001,
	LCT002,
	LCT003,
	LLC001,
	LLC005,
	LLC006,
	LLC007,
	LLC011,
	LLC012,
	LLC013,
	LST001,
};


class hue_controller_t
{
private:
	/// Philips Hue IP
	char* _hue_ip;
	/// Number of lamps
	uint8_t _number_lamps;

public:
	/**
	 * @brief Initialize
	 * @param hue_ip [in] : Philips hue bridge IP
	 * @param n_lamp [in] : Number of lamps
	 */
	hue_controller_t(const char* hue_ip, const uint8_t n_lamp);

	/**
	 * @brief Destructor, free IP buffer
	 */
	~hue_controller_t(void);

	/**
	 * @brief Compute a color from a RGB buffer and send it to hue
	 * @param buffer [in] : RGB buffer
	 * @param width [in] : buffer width
	 * @param height [in] : buffer height
	 */
	void apply_dominant_color_from_buffer(const uint8_t* buffer, const size_t width, const size_t height)const;

	/**
	 * Taken and adapted from Philips Hue SDK
	 * https://github.com/PhilipsHue/PhilipsHueSDK-iOS-OSX/blob/master/ApplicationDesignNotes/RGB%20to%20xy%20Color%20conversion.md
	 */
	static point_t calculate_XY_from_RGB_and_model(const rgba_pixel_t* px, const HUE_LAMP_MODEL model);
	static point_t* color_points_for_model(const HUE_LAMP_MODEL model);
	static point_t get_closest_point_to_points(const point_t& pta, const point_t& ptb, const point_t& ptp);
	static bool check_point_in_lamps_reach_with_color_points(const point_t& pt, const point_t* color_points);

private:
	void get_edges(rgba_pixel_t* pixels, const size_t width, const size_t height, std::unordered_multiset<rgba_pixel_t>& left_edge, std::unordered_multiset<rgba_pixel_t>& right_edge, const size_t col)const;
	void get_middle(rgba_pixel_t* pixels, const size_t width, const size_t height, std::unordered_multiset<rgba_pixel_t>& middle, const size_t wh)const;
	void apply_color_to_lamp(std::unordered_multiset<rgba_pixel_t>& edge, const int random_colors_threshold, const int lamp_n)const;
};

#endif /* __NYX_HUE_CONTROLLER_H__ */

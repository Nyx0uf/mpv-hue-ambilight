#ifndef __NYX_HUE_CONTROLLER_H__
#define __NYX_HUE_CONTROLLER_H__


#include <cstdlib>
struct rgb_pixel_t;
struct point_t;


class hue_controller_t
{
private:
	/// Philips Hue IP
	char* _hue_ip;

public:
	/// Constructor
	hue_controller_t(const char* hue_ip);
	/// Destrucor
	~hue_controller_t(void);

	///
	void apply_dominant_color_from_buffer(const uint8_t* buffer, const size_t width, const size_t height)const;

	static point_t calculateXYFromRGBandModel(const rgb_pixel_t& px, const char* model);
	static point_t* colorPointsForModel(const char* model);
	static point_t getClosestPointToPoints(const point_t& pta, const point_t& ptb, const point_t& ptp);
	static double getDistanceBetweenTwoPoints(const point_t& pt1, const point_t& pt2);
	static bool checkPointInLampsReachwithColorPoints(const point_t& pt, const point_t* color_points);
};


#endif /* __NYX_HUE_CONTROLLER_H__ */

#include "point.h"
#include <cmath>


point_t::point_t(void)
{
	x = 0.0;
	y = 0.0;
}

point_t::point_t(const double xx, const double yy)
{
	x = xx;
	y = yy;
}

point_t::point_t(const point_t& pt)
{
	this->x = pt.x;
	this->y = pt.y;
}

double point_t::distance_between_points(const point_t& pt1, const point_t& pt2)
{
	const double dx = pt1.x - pt2.x; // horizontal difference
	const double dy = pt1.y - pt2.y; // vertical difference
	return std::sqrt(dx * dx + dy * dy);
}

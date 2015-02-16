#include "point.h"


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

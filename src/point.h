#ifndef __NYX_POINT_H__
#define __NYX_POINT_H__


struct point_t
{
	/// X pos
	double x;
	/// Y pos
	double y;

	/// Default constructor
	point_t(void) {x = 0.0; y = 0.0;}
	/// Constructor
	point_t(const double xx, const double yy) {x = xx; y = yy;}

	/// Operators overloading
	inline bool operator==(const point_t& pt)const
	{
		return ((x == pt.x) && (y == pt.y));
	}
};

#endif /* __NYX_POINT_H__ */

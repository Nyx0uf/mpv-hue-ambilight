#ifndef __NYX_POINT_H__
#define __NYX_POINT_H__


struct point_t
{
	/// X pos
	double x;
	/// Y pos
	double y;

	/**
	 * @brief Default constructor: x = 0.0, y = 0.0
	 */
	point_t(void);

	/**
	 * @brief Initialize a point
	 * @param xx [in] : X position
	 * @param yy [in] : Y position
	 */
	point_t(const double xx, const double yy);

	/**
	 * @brief Create a copy of another point
	 * @param pt [in] : Point to copy
	 */
	point_t(const point_t& pt);

	/**
	 * Operators overloading
	 */
	inline bool operator==(const point_t& pt)const
	{
		return ((x == pt.x) && (y == pt.y));
	}
};

#endif /* __NYX_POINT_H__ */

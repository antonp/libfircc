#ifndef _COMMON_H_
#define _COMMON_H_

#include "basedefs.h"

namespace firc
{
	struct Point2d
	{
		Point2d():x(0), y(0) { }
		Point2d(mge::int32 in_x, mge::int32 in_y):x(in_x), y(in_y) { }
		Point2d(const mge::Point2d &point):x(point.x), y(point.y) { }
		mge::int32 x;
		mge::int32 y;
	};

	struct Size2d
	{
		Size2d():width(0), height(0) { }
		Size2d(mge::uint32 in_width, mge::uint32 in_height):width(in_width), height(in_height) { }
		Size2d(const mge::Size2d &size):width(size.width), height(size.height) { }
		mge::uint32 width;
		mge::uint32 height;
	};

	struct Rect
	{
		Rect():top(0), left(0), bottom(0), right(0) { }
		mge::int32 top;
		mge::int32 left;
		mge::int32 bottom;
		mge::int32 right;
	};

	static mge::bool32 pointInRect(const Point2d &point, const Rect &rect)
	{
		return ( point.x > rect.left
			&& point.x < rect.right
			&& point.y > rect.top
			&& point.y < rect.bottom );
	}
}

#endif // _COMMON_H_

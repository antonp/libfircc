#ifndef _MGEINPUT_H_
#define _MGEINPUT_H_

/** @file mgeinput.h
This header file defines some useful input-related constants
*/

#include "basedefs.h"
#include "common.h"

namespace firc
{
	enum MouseButton
	{
		MOUSEBUTTON_LEFT	= (1<<0),
		MOUSEBUTTON_MIDDLE	= (1<<1),
		MOUSEBUTTON_RIGHT	= (1<<2),
		MOUSEBUTTON_THUMB1	= (1<<3),
		MOUSEBUTTON_THUMB2	= (1<<4),
		MOUSEBUTTON_EXTRA1	= (1<<5),
		MOUSEBUTTON_EXTRA2	= (1<<6),
		MOUSEBUTTON_WHEEL=MOUSEBUTTON_MIDDLE
	};

	enum MouseEventType
	{
		MOUSEBUTTON_DOWN,
		MOUSEBUTTON_UP,
		MOUSEMOVE
	};

	struct MouseEvent
	{
		MouseEventType type;
		union
		{
			MouseButton button;
			mge::uint32 buttonState;
		};
		mge::Point2d position;
		mge::int32 xrel;
		mge::int32 yrel;
	};
}

#endif // _MGEINPUT_H_

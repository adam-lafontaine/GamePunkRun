#pragma once

// activate buttons to accept input
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 1
#define MOUSE_X1 0
#define MOUSE_X2 0

// track mouse position
#define MOUSE_POSITION 1

#define MOUSE_WHEEL 1


namespace input
{
#ifdef NO_MOUSE
	constexpr unsigned N_MOUSE_BUTTONS = 0;
#else

	constexpr unsigned N_MOUSE_BUTTONS =
	MOUSE_LEFT + 
	MOUSE_RIGHT + 
	MOUSE_MIDDLE + 
	MOUSE_X1 + 
	MOUSE_X2;

#endif
}


/* mouse */

namespace input
{
	class MouseInput
	{
	public:

		u64 window_id;

	#if MOUSE_POSITION

		Point2Di32 window_pos;

	#endif
	#if MOUSE_WHEEL

		Vec2Di32 wheel;

	#endif

		union
		{
			ButtonState buttons[N_MOUSE_BUTTONS];
			struct
			{
			#if MOUSE_LEFT
				ButtonState btn_left;
			#endif
			#if MOUSE_RIGHT
				ButtonState btn_right;
			#endif
			#if MOUSE_MIDDLE
				ButtonState btn_middle;
			#endif
			#if MOUSE_X1
				ButtonState btn_x1;
			#endif
			#if MOUSE_X2
				ButtonState btn_x2;
			#endif
			};
		};

	};
}


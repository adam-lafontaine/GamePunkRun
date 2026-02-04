#pragma once

#define JOYSTICK_BTN_0 1
#define JOYSTICK_BTN_1 1
#define JOYSTICK_BTN_2 1
#define JOYSTICK_BTN_3 1
#define JOYSTICK_BTN_4 0
#define JOYSTICK_BTN_5 0
#define JOYSTICK_BTN_6 0
#define JOYSTICK_BTN_7 0
#define JOYSTICK_BTN_8 0
#define JOYSTICK_BTN_9 0

#define JOYSTICK_AXIS_0 1
#define JOYSTICK_AXIS_1 0
#define JOYSTICK_AXIS_2 0
#define JOYSTICK_AXIS_3 0
#define JOYSTICK_AXIS_4 0
#define JOYSTICK_AXIS_5 0


namespace input
{

#ifdef NO_JOYSTICK
    constexpr unsigned N_JOYSTICK_BUTTONS = 0;
    constexpr unsigned N_JOYSTICK_AXES = 0;
#else

    constexpr unsigned N_JOYSTICK_BUTTONS = 
    JOYSTICK_BTN_0 +
    JOYSTICK_BTN_1 +
    JOYSTICK_BTN_2 +
    JOYSTICK_BTN_3 +
    JOYSTICK_BTN_4 +
    JOYSTICK_BTN_5 +
    JOYSTICK_BTN_6 +
    JOYSTICK_BTN_7 +
    JOYSTICK_BTN_8 +
    JOYSTICK_BTN_9;

    constexpr unsigned N_JOYSTICK_AXES = 
    JOYSTICK_AXIS_0 +
    JOYSTICK_AXIS_1 +
    JOYSTICK_AXIS_2 +
    JOYSTICK_AXIS_3 +
    JOYSTICK_AXIS_4 +
    JOYSTICK_AXIS_5;

#endif
}


/* joystick */

namespace input
{
	class JoystickInput
	{
	public:
	
		u64 handle = 0;
	
		union
		{
			ButtonState buttons[N_JOYSTICK_BUTTONS];

			struct
			{
			#if JOYSTICK_BTN_0
				ButtonState btn_0;
			#endif
			#if JOYSTICK_BTN_1
				ButtonState btn_1;
			#endif
			#if JOYSTICK_BTN_2
				ButtonState btn_2;
			#endif
			#if JOYSTICK_BTN_3
				ButtonState btn_3;
			#endif
			#if JOYSTICK_BTN_4
				ButtonState btn_4;
			#endif
			#if JOYSTICK_BTN_5
				ButtonState btn_5;
			#endif
			#if JOYSTICK_BTN_6
				ButtonState btn_6;
			#endif
			#if JOYSTICK_BTN_7
				ButtonState btn_7;
			#endif
			#if JOYSTICK_BTN_8
				ButtonState btn_8;
			#endif
			#if JOYSTICK_BTN_9
				ButtonState btn_9;
			#endif
			};
		};

		union
		{
			f32 axes[N_JOYSTICK_AXES];

			struct
			{
			#if JOYSTICK_AXIS_0
				f32 axis_0;
			#endif
			#if JOYSTICK_AXIS_1
				f32 axis_1;
			#endif
			#if JOYSTICK_AXIS_2
				f32 axis_2;
			#endif
			#if JOYSTICK_AXIS_3
				f32 axis_3;
			#endif
			#if JOYSTICK_AXIS_4
				f32 axis_4;
			#endif
			#if JOYSTICK_AXIS_5
				f32 axis_5;
			#endif
			};
		};
	};
	
}


#pragma once


#define GAMEPAD_BTN_DPAD_UP 1
#define GAMEPAD_BTN_DPAD_DOWN 1
#define GAMEPAD_BTN_DPAD_LEFT 1
#define GAMEPAD_BTN_DPAD_RIGHT 1
#define GAMEPAD_BTN_START 1
#define GAMEPAD_BTN_BACK 1
#define GAMEPAD_BTN_SOUTH 1
#define GAMEPAD_BTN_EAST 1
#define GAMEPAD_BTN_WEST 1
#define GAMEPAD_BTN_NORTH 1
#define GAMEPAD_BTN_SHOULDER_LEFT 1
#define GAMEPAD_BTN_SHOULDER_RIGHT 1
#define GAMEPAD_BTN_STICK_LEFT 1
#define GAMEPAD_BTN_STICK_RIGHT 1
#define GAMEPAD_AXIS_STICK_LEFT 1
#define GAMEPAD_AXIS_STICK_RIGHT 1
#define GAMEPAD_TRIGGER_LEFT 1
#define GAMEPAD_TRIGGER_RIGHT 1

#if GAMEPAD_BTN_DPAD_UP && GAMEPAD_BTN_DPAD_DOWN && GAMEPAD_BTN_DPAD_LEFT && GAMEPAD_BTN_DPAD_RIGHT
#define GAMEPAD_BTN_DPAD_ALL 1
#else
#define GAMEPAD_BTN_DPAD_ALL 0
#endif




namespace input
{
#ifdef NO_GAMEPAD
    constexpr unsigned N_GAMEPAD_BUTTONS = 0;
    constexpr unsigned N_GAMEPAD_AXES = 0;
#else

    constexpr unsigned N_GAMEPAD_BUTTONS = 
    GAMEPAD_BTN_DPAD_UP +
    GAMEPAD_BTN_DPAD_DOWN +
    GAMEPAD_BTN_DPAD_LEFT +
    GAMEPAD_BTN_DPAD_RIGHT +
    GAMEPAD_BTN_START +
    GAMEPAD_BTN_BACK +    
    GAMEPAD_BTN_SOUTH +
    GAMEPAD_BTN_EAST +
    GAMEPAD_BTN_WEST +
    GAMEPAD_BTN_NORTH +
    GAMEPAD_BTN_SHOULDER_LEFT +
    GAMEPAD_BTN_SHOULDER_RIGHT +
    GAMEPAD_BTN_STICK_LEFT +
    GAMEPAD_BTN_STICK_RIGHT;

    constexpr unsigned N_GAMEPAD_AXES =
    GAMEPAD_AXIS_STICK_LEFT +
    GAMEPAD_AXIS_STICK_RIGHT +
    GAMEPAD_TRIGGER_LEFT +
    GAMEPAD_TRIGGER_RIGHT;

#endif
}


/* gamepad */

namespace input
{
    class GamepadInput
    {
    public:
    
		u64 handle = 0;
	
        union
        {
            ButtonState buttons[N_GAMEPAD_BUTTONS];

            struct
            {
			#if GAMEPAD_BTN_DPAD_UP
                ButtonState btn_dpad_up;
			#endif
			#if GAMEPAD_BTN_DPAD_DOWN
                ButtonState btn_dpad_down;
			#endif
			#if GAMEPAD_BTN_DPAD_LEFT
                ButtonState btn_dpad_left;
			#endif
			#if GAMEPAD_BTN_DPAD_RIGHT
                ButtonState btn_dpad_right;
			#endif
			#if GAMEPAD_BTN_START
                ButtonState btn_start;
			#endif
			#if GAMEPAD_BTN_BACK
                ButtonState btn_back;
			#endif
			#if GAMEPAD_BTN_SOUTH
                ButtonState btn_south;
			#endif
			#if GAMEPAD_BTN_EAST
                ButtonState btn_east;
			#endif
			#if GAMEPAD_BTN_WEST
                ButtonState btn_west;
			#endif
			#if GAMEPAD_BTN_NORTH
                ButtonState btn_north;
			#endif
			#if GAMEPAD_BTN_SHOULDER_LEFT
                ButtonState btn_shoulder_left;
			#endif
			#if GAMEPAD_BTN_SHOULDER_RIGHT
                ButtonState btn_shoulder_right;
			#endif
			#if GAMEPAD_BTN_STICK_LEFT
                ButtonState btn_stick_left;
			#endif
			#if GAMEPAD_BTN_STICK_RIGHT
                ButtonState btn_stick_right;
			#endif
            };            
        };

	#if GAMEPAD_AXIS_STICK_LEFT
        VectorState<f32> stick_left;
	#endif
	#if GAMEPAD_AXIS_STICK_RIGHT
        VectorState<f32> stick_right;
	#endif
	#if GAMEPAD_TRIGGER_LEFT
        f32 trigger_left;
	#endif
	#if GAMEPAD_TRIGGER_RIGHT
        f32 trigger_right;
	#endif
	#if GAMEPAD_BTN_DPAD_ALL
		VectorState<i8> vec_dpad;
	#endif	

    };
	
}


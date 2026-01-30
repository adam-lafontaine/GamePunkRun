#pragma once

//#define NO_KEYBOARD
//#define NO_MOUSE
//#define NO_GAMEPAD
//#define NO_JOYSTICK
//#DEFINE NO_TOUCH

#define SINGLE_GAMEPAD
#define SINGLE_JOYSTICK

#include "../../util/types.hpp"



/* button state */

namespace input
{
	union ButtonState
	{
		b8 states[3];
		struct
		{
			b8 pressed;
			b8 is_down;
			b8 raised;
		};
	};


	template <typename T>
	class VectorState
	{
	public:
		Vec2D<T> vec;

		f32 magnitude;

		union
		{
			Vec2Df32 unit;

			struct
			{
				f32 cosine;
				f32 sine;
			};
		};		
	};
}


#include "keyboard_input.hpp"
#include "mouse_input.hpp"
#include "gamepad_input.hpp"
#include "joystick_input.hpp"
#include "touch_input.hpp"



/* input */

namespace input
{
#ifdef SINGLE_GAMEPAD
	constexpr u32 MAX_GAMEPADS = 1;
#else
	constexpr u32 MAX_GAMEPADS = 4;
#endif


#ifdef SINGLE_JOYSTICK
	constexpr u32 MAX_JOYSTICKS = 1;
#else
	constexpr u32 MAX_JOYSTICKS = 4;
#endif


	class Input
	{
	public:
		KeyboardInput keyboard;
		MouseInput mouse;

		u64 frame;
		f32 dt_frame;

		union 
		{
			b32 flags = 0;

			struct
			{
				b8 window_size_changed;
				b8 cmd_end_program;
			};
		};
		
	#ifdef SINGLE_GAMEPAD

		union
		{
			GamepadInput gamepad;

			GamepadInput gamepads[MAX_GAMEPADS];
		};		
		
	#else
		GamepadInput gamepads[MAX_GAMEPADS];		
	#endif


	#ifdef SINGLE_JOYSTICK

		union
		{
			JoystickInput joystick;
			JoystickInput joysticks[MAX_JOYSTICKS];
		};

	#else
		JoystickInput joysticks[MAX_JOYSTICKS];
	#endif

		TouchInput touch;

		Input(){}
	};


	class InputArray
	{
	private:
		b8 p = 0;
		b8 c = 1;

		Input inputs[2];

	public:	
		
		u32 n_gamepads = 0;
		u32 n_joysticks = 0;

		Input& prev() { return inputs[p]; }

		Input& curr() { return inputs[c]; }

		void swap() { p = c; c = !p; }
	};
}



/* api */

namespace input
{
	using event_cb = void (*)(void* event);


	bool init(InputArray& inputs);

	void close();

	void record_input(InputArray& inputs);

	void record_input(InputArray& inputs, event_cb handle_event);

}

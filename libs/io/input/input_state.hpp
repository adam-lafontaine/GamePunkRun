#pragma once

#include "input.hpp"
#include "../../math/math.hpp"


/* helpers */

namespace input
{
	
	inline void record_button_input(ButtonState const& old_state, ButtonState& new_state, b32 is_down)
	{
		new_state.pressed = !old_state.is_down && is_down;
		new_state.is_down = is_down;
		new_state.raised = old_state.is_down && !is_down;
	}


	inline void record_button_input(ButtonState& state, b32 is_down)
	{
		state.pressed = !state.is_down && is_down;
		state.raised = state.is_down && !is_down;
		state.is_down = is_down;		
	}


	inline void copy_button_state(ButtonState const& src, ButtonState& dst)
	{
		dst.is_down = src.is_down;
		dst.pressed = 0;
		dst.raised = 0;
	}


	inline void reset_button_state(ButtonState& btn)
	{
		btn.is_down = 0;
		btn.pressed = 0;
		btn.raised = 0;
	}


	template <typename T>
	inline void copy_vec_2d(Vec2D<T> const& src, Vec2D<T>& dst)
	{
		dst.x = src.x;
		dst.y = src.y;
	}


	template <typename T>
	inline void copy_vector_state(VectorState<T> const& src, VectorState<T>& dst)
	{
		copy_vec_2d(src.vec, dst.vec);
		dst.magnitude = src.magnitude;
		copy_vec_2d(src.unit, dst.unit);		
	}


	template <typename T>
	inline void reset_vec_2d(Vec2D<T>& vec)
	{
		vec.x = (T)0;
		vec.y = (T)0;
	}


	template <typename T>
	inline void reset_vector_state(VectorState<T>& vs)
	{
		reset_vec_2d(vs.vec);
		vs.magnitude = 0.0f;
		reset_vec_2d(vs.unit);
	}


    static void set_vector_state(input::VectorState<f32>& vs)
    {
        auto vec = vs.vec;
        auto& unit = vs.unit;

        vs.magnitude = math::magnitude(vec);

        auto mag = vs.magnitude > 0.0f ? vs.magnitude : 1.0f;

        unit.x = vec.x / mag;
        unit.y = vec.y / mag;
    }


    static void set_unit_vector_state(input::VectorState<i8>& vs, int x, int y)
    {
        auto& vec = vs.vec;
        auto& unit = vs.unit;

        vec.x = math::cxpr::sign_i8(x);
        vec.y = math::cxpr::sign_i8(y);

        unit.x = (f32)vec.x;
        unit.y = (f32)vec.y;

        constexpr f32 hypot = 1.4142135f;
        constexpr f32 i_hypot = 1.0f / hypot;
        
        auto mag = (x || y) ? 1.0f : 0.0f;
        auto i_mag = (x && y) ? i_hypot : (x || y) ? 1.0f : 0.0f;

        vs.magnitude = mag;
        unit.x *= i_mag;
        unit.y *= i_mag;
    }
    
    
    static void set_gamepad_dpad_vector(GamepadInput& gamepad)
    {
    #if GAMEPAD_BTN_DPAD_ALL

        int x = gamepad.btn_dpad_right.is_down - gamepad.btn_dpad_left.is_down;
        int y = gamepad.btn_dpad_down.is_down - gamepad.btn_dpad_up.is_down;

        set_unit_vector_state(gamepad.vec_dpad, x, y);
    #endif
    }
}


/* keyboard */

namespace input
{
	inline void copy_keyboard_state(KeyboardInput const& src, KeyboardInput& dst)
	{
		for (u32 i = 0; i < N_KEYBOARD_KEYS; ++i)
		{
			copy_button_state(src.keys[i], dst.keys[i]);
		}
	}


	inline void reset_keyboard_state(KeyboardInput& kbd)
	{
		for (u32 i = 0; i < N_KEYBOARD_KEYS; i++)
		{
			reset_button_state(kbd.keys[i]);
		}
	}
}


/* mouse */

namespace input
{	
	inline void copy_mouse_position(MouseInput const& src, MouseInput& dst)
	{
	#if MOUSE_POSITION
		copy_vec_2d(src.window_pos, dst.window_pos);
	#endif
	}


	inline void reset_mouse_position(MouseInput& mouse)
	{
	#if MOUSE_POSITION
		reset_vec_2d(mouse.window_pos);
	#endif
	}


	inline void reset_mouse_wheel(MouseInput& mouse)
	{
	#if MOUSE_WHEEL
		reset_vec_2d(mouse.wheel);
	#endif
	}	


	inline void copy_mouse_state(MouseInput const& src, MouseInput& dst)
	{
		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}

		copy_mouse_position(src, dst);
		reset_mouse_wheel(dst);
	}


	inline void reset_mouse_state(MouseInput& mouse)
	{
		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
		{
			reset_button_state(mouse.buttons[i]);
		}

		reset_mouse_position(mouse);
		reset_mouse_wheel(mouse);
	}

	
}


/* gamepad */

namespace input
{
	inline void copy_gamepad_axes(GamepadInput const& src, GamepadInput& dst)
	{
	#if GAMEPAD_AXIS_STICK_LEFT
		copy_vector_state(src.stick_left, dst.stick_left);
	#endif

	#if GAMEPAD_AXIS_STICK_RIGHT
		copy_vector_state(src.stick_right, dst.stick_right);
	#endif
	}


	inline void reset_gamepad_axes(GamepadInput& gamepad)
	{
	#if GAMEPAD_AXIS_STICK_LEFT
		reset_vector_state(gamepad.stick_left);
	#endif

	#if GAMEPAD_AXIS_STICK_RIGHT
		reset_vector_state(gamepad.stick_right);
	#endif
	}


	inline void copy_gamepad_triggers(GamepadInput const& src, GamepadInput& dst)
	{
	#if GAMEPAD_TRIGGER_LEFT
		dst.trigger_left = src.trigger_left;
	#endif

	#if GAMEPAD_TRIGGER_RIGHT
		dst.trigger_right = src.trigger_right;
	#endif
	}


	inline void reset_gamepad_triggers(GamepadInput& gamepad)
	{
	#if GAMEPAD_TRIGGER_LEFT
		gamepad.trigger_left = 0.0f;
	#endif

	#if GAMEPAD_TRIGGER_RIGHT
		gamepad.trigger_right = 0.0f;
	#endif
	}


	inline void copy_gamepad_state(GamepadInput const& src, GamepadInput& dst)
	{
		for (u32 i = 0; i < N_GAMEPAD_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}
		
		copy_gamepad_axes(src, dst);
		copy_gamepad_triggers(src, dst);
	}


	inline void reset_gamepad_state(GamepadInput& gamepad)
	{
		for (u32 i = 0; i < N_GAMEPAD_BUTTONS; ++i)
		{
			reset_button_state(gamepad.buttons[i]);
		}

		reset_gamepad_axes(gamepad);
		reset_gamepad_triggers(gamepad);
	}
	
}


/* joystick */

namespace input
{
	inline void copy_joystick_state(JoystickInput const& src, JoystickInput& dst)
	{
		for (u32 i = 0; i < N_JOYSTICK_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}

		for (u32 i = 0; i < N_JOYSTICK_AXES; i++)
		{
			dst.axes[i] = 0.0f;
		}
	}


	inline void reset_joystick_state(JoystickInput& jsk)
	{
		for (u32 i = 0; i < N_JOYSTICK_BUTTONS; ++i)
		{
			reset_button_state(jsk.buttons[i]);
		}

		for (u32 i = 0; i < N_JOYSTICK_AXES; i++)
		{
			jsk.axes[i] = 0.0f;
		}
	}
}


/* touch */

namespace input
{
	inline void copy_touch_gesture(TouchGesture const& src, TouchGesture& dst)
	{
		constexpr Point2Df32 zero = { 0 };

		copy_button_state(src.btn_touch, dst.btn_touch);

		dst.device_id = src.device_id;
		dst.gesture_id = src.device_id ? src.gesture_id : 0;
		dst.pos = src.device_id ? src.pos : zero;
	}
	
	
	inline void copy_touch_state(TouchInput const& src, TouchInput& dst)
	{
		for (u32 i = 0; i < TouchInput::count; i++)
		{
			auto& s = src.gestures[i];
			auto& d = dst.gestures[i];
			copy_touch_gesture(s, d);			
		}
	}


	inline void reset_touch_gesture(TouchGesture& dst)
	{
		reset_button_state(dst.btn_touch);

		dst.device_id = 0;
		dst.gesture_id = 0;
		dst.pos = { 0 };
	}


	inline void reset_touch_state(TouchInput& dst)
	{
		for (u32 i = 0; i < TouchInput::count; i++)
		{
			reset_touch_gesture(dst.gestures[i]);
		}
	}
}


/* copy/reset input */

namespace input
{
	inline void copy_input_state(Input const& src, Input& dst)
	{
		copy_keyboard_state(src.keyboard, dst.keyboard);
		copy_mouse_state(src.mouse, dst.mouse);

		for (u32 i = 0; i < MAX_GAMEPADS; i++)
		{
			copy_gamepad_state(src.gamepads[i], dst.gamepads[i]);
		}

		for (u32 i = 0; i < MAX_JOYSTICKS; i++)
		{
			copy_joystick_state(src.joysticks[i], dst.joysticks[i]);
		}

		copy_touch_state(src.touch, dst.touch);
	}


	inline void reset_input_state(Input& input)
	{
		input.dt_frame = 0.0f;
		input.frame = (u64)0 - 1;
		input.window_size_changed = 0;
		
		reset_keyboard_state(input.keyboard);
		reset_mouse_state(input.mouse);

		for (u32 i = 0; i < MAX_GAMEPADS; i++)
		{
			reset_gamepad_state(input.gamepads[i]);
		}

		for (u32 i = 0; i < MAX_JOYSTICKS; i++)
		{
			reset_joystick_state(input.joysticks[i]);
		}

		reset_touch_state(input.touch);
	}
}
#pragma once

#include "input.hpp"
#include "../../util/numeric.hpp"


/* helpers */

namespace input
{
	namespace num = numeric;

	
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
}


/* keyboard */

namespace input
{
	inline void set_is_active(KeyboardInput& kbd)
	{
		kbd.is_active = false;

		for (u32 i = 0; i < N_KEYBOARD_KEYS; ++i)
		{
			kbd.is_active |= kbd.keys[i].is_down;
		}
	}


	inline void copy_keyboard_state(KeyboardInput const& src, KeyboardInput& dst)
	{
		for (u32 i = 0; i < N_KEYBOARD_KEYS; ++i)
		{
			copy_button_state(src.keys[i], dst.keys[i]);
		}

		set_is_active(dst);
	}


	inline void reset_keyboard_state(KeyboardInput& kbd)
	{
		for (u32 i = 0; i < N_KEYBOARD_KEYS; i++)
		{
			reset_button_state(kbd.keys[i]);
		}

		kbd.is_active = false;
	}
}


/* mouse */

namespace input
{
	inline void set_is_active(MouseInput& mouse)
	{
		// .is_active is initially set in event listener

	#if MOUSE_POSITION

	#endif
	#if MOUSE_WHEEL

	#endif

		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
		{
			mouse.is_active |= mouse.buttons[i].is_down;
		}
	}

	
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

		dst.is_active = false;
		set_is_active(dst);
	}


	inline void reset_mouse_state(MouseInput& mouse)
	{
		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
		{
			reset_button_state(mouse.buttons[i]);
		}

		reset_mouse_position(mouse);
		reset_mouse_wheel(mouse);
		mouse.is_active = false;
	}

	
}


/* gamepad */

namespace input
{
	inline void set_is_active(GamepadInput& gamepad)
    {
        gamepad.is_active = false ||

	#if GAMEPAD_TRIGGER_LEFT
        gamepad.trigger_left != 0.0f ||
	#endif
	#if GAMEPAD_TRIGGER_RIGHT
        gamepad.trigger_right != 0.0f ||
	#endif

	#if GAMEPAD_AXIS_STICK_LEFT
        gamepad.stick_left.vec.x != 0.0f ||
		gamepad.stick_left.vec.y != 0.0f ||
	#endif
	#if GAMEPAD_AXIS_STICK_RIGHT
        gamepad.stick_right.vec.x != 0.0f ||
		gamepad.stick_right.vec.y != 0.0f ||
	#endif
        false;

		if (!gamepad.is_active)
		{
			for (u32 i = 0; i < N_GAMEPAD_BUTTONS; i++)
			{
				gamepad.is_active |= gamepad.buttons[i].is_down;
			}
		}
    }


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
		
		reset_gamepad_axes(dst);
		reset_gamepad_triggers(dst);
		dst.is_active = false;
	}


	inline void reset_gamepad_state(GamepadInput& gamepad)
	{
		for (u32 i = 0; i < N_GAMEPAD_BUTTONS; ++i)
		{
			reset_button_state(gamepad.buttons[i]);
		}

		reset_gamepad_axes(gamepad);
		reset_gamepad_triggers(gamepad);

		gamepad.is_active = false;
		set_is_active(gamepad);
	}
	
}


/* joystick */

namespace input
{
	inline void set_is_active(JoystickInput& jsk)
	{
		jsk.is_active = false;

		for (u32 i = 0; i < N_JOYSTICK_BUTTONS; ++i)
		{
			jsk.is_active |= jsk.buttons[i].is_down;
		}

		for (u32 i = 0; i < N_GAMEPAD_AXES; ++i)
		{
			jsk.is_active |= num::abs(jsk.axes[i]) > 0.001f;
		}
	}


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

		set_is_active(dst);
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

		jsk.is_active = false;
	}
}


/* copy/reset input */

namespace input
{
	inline void set_is_active(Input& input)
	{
		set_is_active(input.keyboard);
		set_is_active(input.mouse);

		for (u32 i = 0; i < MAX_GAMEPADS; i++)
		{
			set_is_active(input.gamepads[i]);
		}

		for (u32 i = 0; i < MAX_JOYSTICKS; i++)
		{
			set_is_active(input.joysticks[i]);
		}		
	}


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
	}
}
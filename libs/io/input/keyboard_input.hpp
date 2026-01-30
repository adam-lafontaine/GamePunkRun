#pragma once


// activate keys to accept input from
#define KEYBOARD_A 1
#define KEYBOARD_B 1
#define KEYBOARD_C 0
#define KEYBOARD_D 1
#define KEYBOARD_E 0
#define KEYBOARD_F 0
#define KEYBOARD_G 1
#define KEYBOARD_H 0
#define KEYBOARD_I 0
#define KEYBOARD_J 0
#define KEYBOARD_K 0
#define KEYBOARD_L 0
#define KEYBOARD_M 0
#define KEYBOARD_N 0
#define KEYBOARD_O 0
#define KEYBOARD_P 1
#define KEYBOARD_Q 0
#define KEYBOARD_R 1
#define KEYBOARD_S 1
#define KEYBOARD_T 0
#define KEYBOARD_U 0
#define KEYBOARD_V 0
#define KEYBOARD_W 1
#define KEYBOARD_X 0
#define KEYBOARD_Y 0
#define KEYBOARD_Z 0
#define KEYBOARD_0 0
#define KEYBOARD_1 1
#define KEYBOARD_2 1
#define KEYBOARD_3 1
#define KEYBOARD_4 1
#define KEYBOARD_5 0
#define KEYBOARD_6 0
#define KEYBOARD_7 0
#define KEYBOARD_8 0
#define KEYBOARD_9 0
#define KEYBOARD_UP 1
#define KEYBOARD_DOWN 1
#define KEYBOARD_LEFT 1
#define KEYBOARD_RIGHT 1
#define KEYBOARD_RETURN 1
#define KEYBOARD_ESCAPE 0
#define KEYBOARD_SPACE 1
#define KEYBOARD_LSHIFT 0
#define KEYBOARD_RSHIFT 0
#define KEYBOARD_NUMPAD_0 0
#define KEYBOARD_NUMPAD_1 0
#define KEYBOARD_NUMPAD_2 0
#define KEYBOARD_NUMPAD_3 0
#define KEYBOARD_NUMPAD_4 0
#define KEYBOARD_NUMPAD_5 0
#define KEYBOARD_NUMPAD_6 0
#define KEYBOARD_NUMPAD_7 0
#define KEYBOARD_NUMPAD_8 0
#define KEYBOARD_NUMPAD_9 0
#define KEYBOARD_NUMPAD_PLUS 1
#define KEYBOARD_NUMPAD_MINUS 1
#define KEYBOARD_NUMPAD_MULTIPLY 0
#define KEYBOARD_NUMPAD_DIVIDE 0
#define KEYBOARD_CTRL 1


namespace input
{
#ifdef NO_KEYBOARD
	constexpr unsigned N_KEYBOARD_KEYS = 0;
#else

	constexpr unsigned N_KEYBOARD_KEYS = 
	KEYBOARD_A + 
	KEYBOARD_B + 
	KEYBOARD_C + 
	KEYBOARD_D + 
	KEYBOARD_E + 
	KEYBOARD_F + 
	KEYBOARD_G + 
	KEYBOARD_H + 
	KEYBOARD_I + 
	KEYBOARD_J + 
	KEYBOARD_K + 
	KEYBOARD_L + 
	KEYBOARD_M + 
	KEYBOARD_N + 
	KEYBOARD_O + 
	KEYBOARD_P + 
	KEYBOARD_Q + 
	KEYBOARD_R + 
	KEYBOARD_S + 
	KEYBOARD_T + 
	KEYBOARD_U + 
	KEYBOARD_V + 
	KEYBOARD_W + 
	KEYBOARD_X + 
	KEYBOARD_Y + 
	KEYBOARD_Z + 
	KEYBOARD_0 + 
	KEYBOARD_1 + 
	KEYBOARD_2 + 
	KEYBOARD_3 + 
	KEYBOARD_4 + 
	KEYBOARD_5 + 
	KEYBOARD_6 + 
	KEYBOARD_7 + 
	KEYBOARD_8 + 
	KEYBOARD_9 + 
	KEYBOARD_UP + 
	KEYBOARD_DOWN + 
	KEYBOARD_LEFT + 
	KEYBOARD_RIGHT + 
	KEYBOARD_RETURN + 
	KEYBOARD_ESCAPE + 
	KEYBOARD_SPACE + 
	KEYBOARD_LSHIFT +
	KEYBOARD_RSHIFT +
	KEYBOARD_NUMPAD_0 +
	KEYBOARD_NUMPAD_1 +
	KEYBOARD_NUMPAD_2 +
	KEYBOARD_NUMPAD_3 +
	KEYBOARD_NUMPAD_4 +
	KEYBOARD_NUMPAD_5 +
	KEYBOARD_NUMPAD_6 +
	KEYBOARD_NUMPAD_7 +
	KEYBOARD_NUMPAD_8 +
	KEYBOARD_NUMPAD_9 +
	KEYBOARD_NUMPAD_PLUS + 
	KEYBOARD_NUMPAD_MINUS + 
	KEYBOARD_NUMPAD_MULTIPLY + 
	KEYBOARD_NUMPAD_DIVIDE +
	KEYBOARD_CTRL;

#endif
}



/* keyboard */

namespace input
{
	class KeyboardInput
	{
	public:
		
		union
		{
			ButtonState keys[N_KEYBOARD_KEYS];
			
			struct
			{

			#if KEYBOARD_A
				ButtonState kbd_A;
			#endif
			#if KEYBOARD_B
				ButtonState kbd_B;
			#endif
			#if KEYBOARD_C
				ButtonState kbd_C;
			#endif
			#if KEYBOARD_D
				ButtonState kbd_D;
            #endif
            #if KEYBOARD_E
				ButtonState kbd_E;
            #endif
            #if KEYBOARD_F
				ButtonState kbd_F;
            #endif
            #if KEYBOARD_G
				ButtonState kbd_G;
            #endif
            #if KEYBOARD_H
				ButtonState kbd_H;
            #endif
            #if KEYBOARD_I
				ButtonState kbd_I;
            #endif
            #if KEYBOARD_J
				ButtonState kbd_J;
            #endif
            #if KEYBOARD_K
				ButtonState kbd_K;
            #endif
            #if KEYBOARD_L
				ButtonState kbd_L;
            #endif
            #if KEYBOARD_M
				ButtonState kbd_M;
            #endif
            #if KEYBOARD_N
				ButtonState kbd_N;
            #endif
            #if KEYBOARD_O
				ButtonState kbd_O;
            #endif
            #if KEYBOARD_P
				ButtonState kbd_P;
            #endif
            #if KEYBOARD_Q
				ButtonState kbd_Q;
            #endif
            #if KEYBOARD_R
				ButtonState kbd_R;
            #endif
            #if KEYBOARD_S
				ButtonState kbd_S;
            #endif
            #if KEYBOARD_T
				ButtonState kbd_T;
            #endif
            #if KEYBOARD_U
				ButtonState kbd_U;
            #endif
            #if KEYBOARD_V
				ButtonState kbd_V;
            #endif
            #if KEYBOARD_W
				ButtonState kbd_W;
            #endif
            #if KEYBOARD_X
				ButtonState kbd_X;
            #endif
            #if KEYBOARD_Y
				ButtonState kbd_Y;
            #endif
            #if KEYBOARD_Z
				ButtonState kbd_Z;
            #endif
            #if KEYBOARD_0
				ButtonState kbd_0;
            #endif
            #if KEYBOARD_1
				ButtonState kbd_1;
            #endif
            #if KEYBOARD_2
				ButtonState kbd_2;
            #endif
            #if KEYBOARD_3
				ButtonState kbd_3;
            #endif
            #if KEYBOARD_4
				ButtonState kbd_4;
            #endif
            #if KEYBOARD_5
				ButtonState kbd_5;
            #endif
            #if KEYBOARD_6
				ButtonState kbd_6;
            #endif
            #if KEYBOARD_7
				ButtonState kbd_7;
            #endif
            #if KEYBOARD_8
				ButtonState kbd_8;
            #endif
            #if KEYBOARD_9
				ButtonState kbd_9;
            #endif
            #if KEYBOARD_UP
				ButtonState kbd_up;
            #endif
            #if KEYBOARD_DOWN
				ButtonState kbd_down;
            #endif
            #if KEYBOARD_LEFT
				ButtonState kbd_left;
            #endif
            #if KEYBOARD_RIGHT
				ButtonState kbd_right;
            #endif
            #if KEYBOARD_RETURN
				ButtonState kbd_return;
            #endif
            #if KEYBOARD_ESCAPE
				ButtonState kbd_escape;
            #endif
            #if KEYBOARD_SPACE
				ButtonState kbd_space;
            #endif
            #if KEYBOARD_LSHIFT
				ButtonState kbd_left_shift;
            #endif
            #if KEYBOARD_RSHIFT
				ButtonState kbd_right_shift;
            #endif
            #if KEYBOARD_NUMPAD_0
				ButtonState npd_0;
            #endif
            #if KEYBOARD_NUMPAD_1
				ButtonState npd_1;
            #endif
            #if KEYBOARD_NUMPAD_2
				ButtonState npd_2;
            #endif
            #if KEYBOARD_NUMPAD_3
				ButtonState npd_3;
            #endif
            #if KEYBOARD_NUMPAD_4
				ButtonState npd_4;
            #endif
            #if KEYBOARD_NUMPAD_5
				ButtonState npd_5;
            #endif
            #if KEYBOARD_NUMPAD_6
				ButtonState npd_6;
            #endif
            #if KEYBOARD_NUMPAD_7
				ButtonState npd_7;
            #endif
            #if KEYBOARD_NUMPAD_8
				ButtonState npd_8;
            #endif
            #if KEYBOARD_NUMPAD_9
				ButtonState npd_9;
            #endif
            #if KEYBOARD_NUMPAD_PLUS
				ButtonState npd_plus;
            #endif
            #if KEYBOARD_NUMPAD_MINUS
				ButtonState npd_minus;
            #endif
            #if KEYBOARD_NUMPAD_MULTIPLY
				ButtonState npd_mult;
            #endif
            #if KEYBOARD_NUMPAD_DIVIDE
				ButtonState npd_div;
            #endif
            #if KEYBOARD_CTRL
				ButtonState kbd_ctrl;
			#endif

			};

		};
	};
}

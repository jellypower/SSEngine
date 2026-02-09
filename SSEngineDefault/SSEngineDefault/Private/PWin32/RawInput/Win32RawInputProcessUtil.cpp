#include "Win32RawInputProcessUtil.h"


EKeyCode WindowsVKCodeToSSKeyCode(WPARAM wParam)
{
	switch (wParam)
	{
	case 'A': return EKeyCode::KEY_A;
	case 'B': return EKeyCode::KEY_B;
	case 'C': return EKeyCode::KEY_C;
	case 'D': return EKeyCode::KEY_D;
	case 'E': return EKeyCode::KEY_E;
	case 'F': return EKeyCode::KEY_F;
	case 'G': return EKeyCode::KEY_G;
	case 'H': return EKeyCode::KEY_H;
	case 'I': return EKeyCode::KEY_I;
	case 'J': return EKeyCode::KEY_J;
	case 'K': return EKeyCode::KEY_K;
	case 'L': return EKeyCode::KEY_L;
	case 'M': return EKeyCode::KEY_M;
	case 'N': return EKeyCode::KEY_N;
	case 'O': return EKeyCode::KEY_O;
	case 'P': return EKeyCode::KEY_P;
	case 'Q': return EKeyCode::KEY_Q;
	case 'R': return EKeyCode::KEY_R;
	case 'S': return EKeyCode::KEY_S;
	case 'T': return EKeyCode::KEY_T;
	case 'U': return EKeyCode::KEY_U;
	case 'V': return EKeyCode::KEY_V;
	case 'W': return EKeyCode::KEY_W;
	case 'X': return EKeyCode::KEY_X;
	case 'Y': return EKeyCode::KEY_Y;
	case 'Z': return EKeyCode::KEY_Z;

	case VK_UP: return EKeyCode::KEY_UP;
	case VK_DOWN: return EKeyCode::KEY_DOWN;
	case VK_LEFT: return EKeyCode::KEY_LEFT;
	case VK_RIGHT: return EKeyCode::KEY_RIGHT;

	case VK_CONTROL: return EKeyCode::KEY_Ctrl;
	}

	return EKeyCode::Unknown;
}

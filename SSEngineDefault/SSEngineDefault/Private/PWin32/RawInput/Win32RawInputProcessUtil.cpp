#include "Win32RawInputProcessUtil.h"


EKeyCode WindowsVKCodeToSSKeyCode(WPARAM wParam)
{
	switch (wParam)
	{
	case 'A': return EKeyCode::KEY_A;
	case 'C': return EKeyCode::KEY_C;
	case 'D': return EKeyCode::KEY_D;
	case 'E': return EKeyCode::KEY_E;
	case 'Q': return EKeyCode::KEY_Q;
	case 'S': return EKeyCode::KEY_S;
	case 'W': return EKeyCode::KEY_W;
	case 'Z': return EKeyCode::KEY_Z;
	case 'M': return EKeyCode::KEY_M;
	case 'N': return EKeyCode::KEY_N;

	case VK_UP: return EKeyCode::KEY_UP;
	case VK_DOWN: return EKeyCode::KEY_DOWN;
	case VK_LEFT: return EKeyCode::KEY_LEFT;
	case VK_RIGHT: return EKeyCode::KEY_RIGHT;
	}

	return EKeyCode::Unknown;
}

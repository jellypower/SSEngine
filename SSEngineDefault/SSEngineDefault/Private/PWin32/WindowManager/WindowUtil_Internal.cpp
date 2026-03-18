#include "WindowUtil_Internal.h"

#include "SSEngineDefault/Public/RawInput/RawInputUtils.h"


void ShowCursorForce_Internal(bool bVisible)
{
	if (bVisible)
	{
		while (::ShowCursor(true) < 0);
	}
	else
	{
		while (::ShowCursor(false) >= 0);
	}
}

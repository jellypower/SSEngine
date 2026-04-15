#pragma once
#include "IWindowManager.h"
#include "SSEngineDefault/Public/GlobalVariableSet/GlobalVariableSet.h"

class SSMainWindowInfo
{
public:
	static bool IsFocusingWindowForcingCenter()
	{
		if (g_MainWindowManager == nullptr)
		{
			return false;
		}

		IWindow* Focusing = g_MainWindowManager->GetFocusingWindow();
		if (Focusing == nullptr)
		{
			return false;
		}

		return Focusing->IsForcingMouseCenter();
	}
};

#pragma once
#include "SSEngineDefault/Public/ISSUnknown.h"
#include "SSEngineDefault/Public/SSVector.h"

class IWindow;

class IWindowManager : public ISSUnknown
{
public:
	virtual IWindow* GetMainWindow() const = 0;
	virtual IWindow* GetFocusingWindow() const = 0;

public:
	// TODO: CreateNewWindow랑 HWND 기준으로 FocusingWindow 바꿔주는 기능 만들기
	virtual void ProcessWindowEndOfFrame() = 0;


};

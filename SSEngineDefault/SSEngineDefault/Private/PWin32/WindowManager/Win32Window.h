#pragma once
#include <Windows.h>
#include "SSEngineDefault/Public/WindowManager/IWindow.h"

class Win32Window : public IWindow
{
private:
	HWND _hWnd;

	Vector2i32 _WindowLT;
	Vector2i32 _WindowSize;
	bool _bVisibleMouse = true;
	bool _bForceMouseCenter = false;

public:
	Win32Window(HWND InHwnd, RECT InitWinRect);
	virtual void Release() override;

public:
	virtual void ResizeWindowXXX(uint32 width, uint32 height) override;
	virtual void PerFrame_OnEndOfFrame() override;

public:
	virtual Vector2i32 GetWindowCenter() const override;
	virtual Vector2i32 GetWindowLeftTop() const override;
	virtual Vector2i32 GetWindowSize() const override;
	virtual bool IsMouseVisible() const override;
	virtual bool IsForcingMouseCenter() const override;

	virtual void SetVisibleMouse(bool bVisible) override;
	virtual void SetForceMouseCenter(bool bForce) override;

public:
	HWND GetHWND() const;
};

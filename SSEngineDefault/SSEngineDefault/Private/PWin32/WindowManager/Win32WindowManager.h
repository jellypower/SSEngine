#pragma once
#include <Windows.h>

#include "SSEngineDefault/Public/SSContainer/PooledList.h"
#include "SSEngineDefault/Public/WindowManager/IWindowManager.h"



class Win32WindowManager : public IWindowManager
{
private:
	IWindow* _MainWindow = nullptr;
	IWindow* _FocusingWindow = nullptr;

	HWND _hMainWnd;
	HINSTANCE _hInstance;

	SS::PooledList<IWindow*> _Windows;

public:
	virtual ~Win32WindowManager();

public:
	virtual IWindow* GetMainWindow() const override;
	virtual IWindow* GetFocusingWindow() const override;

public:
	virtual void ProcessWindowEndOfFrame() override;

public:
	void Win32ProcessWindowEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void InitWindowManagerWinHandle(IWindowManager* InWindowManager, HWND hMainWnd, RECT MainWndInitalSize, HINSTANCE hInstance);


private:
	IWindow* FindWindowOfHWND(HWND InHWnd) const;

	void SetFocusWindow(HWND hNewWindow);

};

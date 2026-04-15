#pragma once
#include "SSEngineDefault/Public/INoncopyable.h"
#include "SSEngineDefault/Public/SSVector.h"

class IWindow : public INoncopyable
{
public:
	virtual void ResizeWindowXXX(uint32 width, uint32 height) = 0;

	virtual void PerFrame_OnEndOfFrame() = 0;

public:
	virtual Vector2i32 GetWindowCenter() const = 0;
	virtual Vector2i32 GetWindowLeftTop() const = 0;
	virtual Vector2i32 GetWindowSize() const = 0;
	virtual bool IsMouseVisible() const = 0;
	virtual bool IsForcingMouseCenter() const = 0;

	virtual void SetVisibleMouse(bool bVisible) = 0;
	virtual void SetForceMouseCenter(bool bForce) = 0;
};

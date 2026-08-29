#pragma once
#include "ISSNoncopyable.h"


class ISSUnknown : public ISSNoncopyable
{
public:
	virtual void Release() = 0;
};

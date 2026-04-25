#pragma once
#include "ICollInstanceBase.h"

class ICISphere : public  ICollInstanceBase
{
public:
	virtual float GetRadius() const = 0;
	virtual void SetRadius(float InRadius) = 0;
};

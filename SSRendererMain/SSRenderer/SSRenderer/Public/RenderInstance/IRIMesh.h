#pragma once
#include "IRenderInstance.h"

class IRIMesh : public IRenderInstance
{
public:
	virtual ModelAsset* GetModelAsset() const = 0;
};

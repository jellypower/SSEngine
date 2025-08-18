#pragma once
#include "IRenderInstance.h"

class IModelAsset;

class IRIMesh : public IRenderInstance
{
public:
	virtual IModelAsset* GetModelAsset() const = 0;
	virtual void SetModelAsset(IModelAsset* InAsset) = 0;

};

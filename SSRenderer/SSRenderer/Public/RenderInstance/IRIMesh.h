#pragma once
#include "IRenderInstance.h"

class IModelAsset;

class IRIMesh : public IRenderInstance // TODO: IRIModel으로 이름 바꾸기
{
public:
	virtual IModelAsset* GetModelAsset() const = 0;
	virtual void SetModelAsset(IModelAsset* InAsset) = 0;

};

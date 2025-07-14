#pragma once
#include "IRenderInstance.h"

class IModelAsset;

class IRIMesh : public IRenderInstance // TODO: IRIModel으로 이름 바꾸기
{
public:
	virtual IModelAsset* GetModelAsset() const = 0;
};

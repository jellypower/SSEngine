#pragma once
#include "IRenderInstance.h"

class IModelAsset;
class IMeshAsset;
class IMaterialAsset;

class IRIMesh : public IRenderInstance
{
public:
	virtual IMeshAsset* GetMeshAsset() const = 0;
	virtual IMaterialAsset* GetMaterialAsset(int MtlIdx) const = 0;

	virtual void SetMeshAsset(IMeshAsset* InAsset) = 0;
	virtual void SetMaterialAsset(IMaterialAsset* InAsset, int32 MtlIdx) = 0;
};

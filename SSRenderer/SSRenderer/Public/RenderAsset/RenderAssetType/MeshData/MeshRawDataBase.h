#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class EMeshType
{
	None = 0,
	Rigid = 1,
	Skinned = 2,
};


struct MeshRawDataBase : public INoncopyable
{
public:
	virtual EMeshType GetMeshType() const = 0;
	virtual void ReleaseData() = 0;
};
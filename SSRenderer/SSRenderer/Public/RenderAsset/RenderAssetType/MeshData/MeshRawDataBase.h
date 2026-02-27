#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class EMeshType : int32;

struct MeshRawDataBase : public INoncopyable
{
public:
	virtual EMeshType GetMeshType() const = 0;
	virtual void ReleaseData() = 0;
};
#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

enum class EMeshType
{
	None = 0,
	Rigid = 1,
	Skinned = 2
};


// 날것으로 쓰기 위한 
struct IMeshRawData : public INoncopyable
{
	EMeshType _MeshType = EMeshType::None;
};
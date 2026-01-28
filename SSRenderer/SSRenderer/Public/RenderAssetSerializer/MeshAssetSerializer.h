#pragma once
#include "SSEngineDefault/Public/SSContainer/PooledList.h"


struct MeshRawDataBase;

struct MeshDataSerializerContainer
{
	SS::PooledList<byte> Data;
	MeshRawDataBase* MeshData = nullptr;
};

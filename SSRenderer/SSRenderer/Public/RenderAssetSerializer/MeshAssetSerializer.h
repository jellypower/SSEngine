#pragma once
#include "SSEngineDefault/Public/SSContainer/PooledList.h"



class IMeshAsset;

struct MeshAssetSerializerContainer
{
	SS::PooledList<byte> Data;
	IMeshAsset* MeshAsset = nullptr;
};

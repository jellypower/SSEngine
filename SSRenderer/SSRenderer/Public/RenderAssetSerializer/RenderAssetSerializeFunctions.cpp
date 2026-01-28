#define SSRENDERER_MODULE_EXPORT
#include "RenderAssetSerializeFunctions.h"

#include "MeshAssetSerializer.h"
#include "SSRenderer/Private/RenderAsset/RenderAssetType/MeshAsset.h"

void FillDataFromAsset(MeshAssetSerializerContainer& Container)
{
	Container.Data.Clear();
	MeshAsset* MeshAssetToFill = static_cast<MeshAsset*>(Container.MeshAsset);
}

bool FillAssetFromData(MeshAssetSerializerContainer& Container)
{
	MeshAsset* MeshAssetToFill = static_cast<MeshAsset*>(Container.MeshAsset);
}

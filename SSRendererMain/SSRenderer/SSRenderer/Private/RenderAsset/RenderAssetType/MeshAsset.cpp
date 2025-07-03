#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/IMeshRawData.h"

MeshAsset::MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath) :
	SSAssetBase(EAssetType::Mesh, InAssetName, InAssetPath)
{

}

MeshAsset::~MeshAsset()
{
}

void MeshAsset::ReleaseSystemData()
{
	delete _MeshRawData;
	_MeshRawData = nullptr;
}

void MeshAsset::ReleaseGALData()
{
	delete _GALMeshAsset;
	_GALMeshAsset = nullptr;
}

#include "MeshAsset.h"

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"

MeshAsset::MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, EMeshType InMeshType) :
	SSAssetBase(EAssetType::Mesh, InAssetName, InAssetPath),
	_meshType(InMeshType)

{

}

MeshAsset::~MeshAsset()
{
}

void MeshAsset::ReleaseSystemData()
{
	free(_vertexData);
	_vertexData = nullptr;

	free(_indexData);
	_indexData = nullptr;

	_eachVertexSize = 0;
	_vertexCnt = 0;
	_subMeshCnt = 0;
	_indexDataCnt[SUBMESH_COUNT_MAX] = { 0, };
	_indexDataStartIndex[SUBMESH_COUNT_MAX] = { 0, };
	_wholeIndexDataCnt = 0;
}

void MeshAsset::ReleaseGALData()
{
	delete _GALMeshAsset;
	_GALMeshAsset = nullptr;
}

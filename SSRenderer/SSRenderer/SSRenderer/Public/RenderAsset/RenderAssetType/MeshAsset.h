#pragma once
#include "SSAssetBase.h"
#include "VertexTypes/SSDefaultVertexTypes.h"

class GPUMeshAssetInstanceBase;

enum class EMeshType
{
	None = 0,
	Rigid = 1,
	Skinned = 2
};

struct alignas(16) SSDefaultVertex {
	Vector4f Pos;
	Vector4f Normal;
	Vector4f Tangent;
	Vector2f Uv[VERTEX_UV_MAP_COUNT_MAX];
};

class MeshAsset : public SSAssetBase 
{
	friend class SSFBXImporterUtils;
public:
	GPUMeshAssetInstanceBase* _GPUMeshAsset = nullptr;

protected:
	EMeshType _meshType = EMeshType::None;

	void* _vertexData = nullptr;
	int32 _eachVertexSize = 0;
	int32 _vertexCnt = 0;

	uint8 _subMeshCnt = 0;

	uint32* _indexData = nullptr;
	int32 _indexDataCnt[SUBMESH_COUNT_MAX] = { 0, };
	int32 _indexDataStartIndex[SUBMESH_COUNT_MAX] = { 0, };
	int32 _wholeIndexDataCnt = 0;


public:
	MeshAsset(SS::SHasherW InAssetName, SS::SHasherW InAssetPath, EMeshType InMeshType);
	virtual ~MeshAsset();

public:
	const void* GetVertexData() const { return _vertexData; }
	int32 GetEachVertexSize() const { return _eachVertexSize; }
	int32 GetVertexCnt() const { return _vertexCnt; }

	uint8 GetSubMeshCnt() const { return _subMeshCnt; }

	const uint32* GetIndexData() const { return _indexData; }
	int32 GetIndexDataCnt(int32 SubMeshIdx = 0) const { return _indexDataCnt[SubMeshIdx]; }
	int32 GetIndexDataStartIndex(int32 SubMeshIdx) const { return _indexDataStartIndex[SubMeshIdx]; }
	int32 WholeIndexDataCnt() const { return _wholeIndexDataCnt; }



	void ReleaseSystemData();
	void ReleaseGPUData();

};


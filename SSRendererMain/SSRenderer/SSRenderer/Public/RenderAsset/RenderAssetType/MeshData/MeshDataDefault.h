#pragma once
#include "MeshRawDataBase.h"
#include "SSGAL/Public/SSGALInlineSettings.h"


class MeshRawDataDefault : public MeshRawDataBase
{
public:
	void* _vertexData = nullptr;
	int32 _eachVertexSize = 0;
	int32 _vertexCnt = 0;

	uint8 _subMeshCnt = 0;

	uint32* _indexData = nullptr;
	int32 _indexDataCnt[SUBMESH_COUNT_MAX] = { 0, };
	int32 _indexDataStartIndex[SUBMESH_COUNT_MAX] = { 0, };
	int32 _wholeIndexDataCnt = 0;

	virtual ~MeshRawDataDefault()
	{
		free(_indexData);
		free(_vertexData);
	}
};

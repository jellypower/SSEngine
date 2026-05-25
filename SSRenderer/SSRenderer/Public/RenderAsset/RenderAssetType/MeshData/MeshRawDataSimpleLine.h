#pragma once
#include "MeshDataDefault.h"


class MeshRawDataSimpleLine : public MeshRawDataBase
{
public:
	SimpleLineVertex* _vertexData = nullptr;
	uint32* _indexData = nullptr;

	int32 _vertexCnt = 0;
	int32 _IdxCnt = 0;

public:
	EMeshType GetMeshType() const override
	{
		return EMeshType::SimpleLine;
	}

	void ReleaseData() override
	{
		if (_vertexData != nullptr)
		{
			free(_vertexData);
			_vertexData = nullptr;
		}

		if (_indexData != nullptr)
		{
			free(_indexData);
			_indexData = nullptr;
		}
	}
};

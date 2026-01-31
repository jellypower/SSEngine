#pragma once
#include "MeshRawDataBase.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/RAFileOutline/MeshRawDataHeaders.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"



class MeshRawDataDefault : public MeshRawDataBase
{
public:
	MeshRawDataVertexHeader _VertexHeader;

	void* _vertexData = nullptr;
	uint32* _indexData = nullptr;


	virtual EMeshType GetMeshType() const override
	{
		return EMeshType::Rigid;
	}

	virtual void ReleaseData() override
	{
		if (_indexData != nullptr)
		{
			free(_indexData);
			_indexData = nullptr;
		}

		if (_vertexData != nullptr)
		{
			free(_vertexData);
			_vertexData = nullptr;
		}
	}
};

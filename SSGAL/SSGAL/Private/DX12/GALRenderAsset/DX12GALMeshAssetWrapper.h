#pragma once
#include <d3d12.h>

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"
#include "SSRenderer/Public/RenderCommon/SSRendererInlineSettings.h"

class DX12GALRenderDevice;

class DX12GALMeshAssetWrapper : public GALMeshAssetWrapperBase
{
public:
	DX12GALMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDevice* InOwnerRenderDevice);
	virtual ~DX12GALMeshAssetWrapper();

public:
	ID3D12Resource* _VertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _VertexBufferView = {};

	ID3D12Resource* _IndexBuffer = nullptr;
	int32 _SubMeshCnt = 0;
	D3D12_INDEX_BUFFER_VIEW _IndexBufferView[SUBMESH_COUNT_MAX];
};

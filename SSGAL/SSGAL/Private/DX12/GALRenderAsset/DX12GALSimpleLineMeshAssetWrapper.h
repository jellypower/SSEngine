#pragma once

#include "SSGAL/Public/GALRenderAsset/GALMeshAssetWrapperBase.h"

class DX12GALRenderDeviceContext;
class DX12GALRenderDevice;

class DX12GALSimpleLineMeshAssetWrapper : public GALMeshAssetWrapperBase
{
public:
	DX12GALSimpleLineMeshAssetWrapper(IMeshAsset* ownerMeshAsset, DX12GALRenderDeviceContext* ResourceUpdateExecutor);
	void Release() override;

public:
	DX12GALRenderDevice* _OwnerRenderDevice = nullptr;

	ID3D12Resource* _VertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _VertexBufferView = {};

	ID3D12Resource* _IndexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW _IndexBufferView = {};
	int32 _IndexCnt = 0;
};

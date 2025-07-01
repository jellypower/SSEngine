#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/RootSignaturePool.h"

class DX12GALRenderDevice;

class DX12RootSignaturePool : public RootSignaturePool
{
public:
	DX12RootSignaturePool(DX12GALRenderDevice* InOwnerRenderDevice);

	virtual void InstantiateAllRootSignatures()		override;
	virtual void ReleaseAllRoogSignatures()			override;
};

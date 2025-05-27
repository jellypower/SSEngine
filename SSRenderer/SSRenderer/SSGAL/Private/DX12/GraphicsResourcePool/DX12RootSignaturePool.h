#pragma once
#include "SSGAL/Public/GraphicsResourcePool/RootSignaturePool.h"

class DX12RootSignaturePool : public RootSignaturePool
{
public:
	virtual void InstantiateAllRootSignatures()		override;
	virtual void ReleaseAllRoogSignatures()			override;
};

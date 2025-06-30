#pragma once
#include "SSGAL/Private/PCommon/GALWrapper/RootSignaturePool.h"

class DX12RootSignaturePool : public RootSignaturePool
{
public:
	virtual void InstantiateAllRootSignatures()		override;
	virtual void ReleaseAllRoogSignatures()			override;
};

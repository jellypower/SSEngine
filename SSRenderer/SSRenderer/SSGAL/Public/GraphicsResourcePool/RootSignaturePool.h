#pragma once
#include "GraphicsResourceWrapper/RootSignatureWrapper.h"
#include "SSEngineDefault/Public/INoncopyable.h"

class RootSignaturePool : public INoncopyable
{
protected:
	RootSignatureWrapper* _rootSignatures[(int32)ERootSignatureType::COUNT] = { nullptr, };

public:
	virtual ~RootSignaturePool();

	const RootSignatureWrapper* GetRootSignature(ERootSignatureType InType) const;

	virtual void InstantiateAllRootSignatures()		= 0;
	virtual void ReleaseAllRoogSignatures()			= 0;
};

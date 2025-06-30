#pragma once
#include "RootSignatureWrapper.h"
#include "SSEngineDefault/Public/INoncopyable.h"

class RootSignaturePool : public INoncopyable
{
protected:
	RootSignatureWrapper* _rootSignatures[(int32)ERootSignatureType::COUNT] = { nullptr, };

public:
	const RootSignatureWrapper* GetRootSignature(ERootSignatureType InType) const { return _rootSignatures[(int32)InType]; }

	virtual void InstantiateAllRootSignatures()		= 0;
	virtual void ReleaseAllRoogSignatures()			= 0;
};

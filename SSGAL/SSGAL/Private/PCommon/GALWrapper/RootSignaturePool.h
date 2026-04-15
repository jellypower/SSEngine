#pragma once
#include "RootSignatureWrapper.h"
#include "Public/GALRenderTarget/GALRTCommonEnums.h"
#include "SSEngineDefault/Public/INoncopyable.h"

class GALRenderDevice;

class RootSignaturePool : public INoncopyable
{
protected:
	GALRenderDevice* _OwnerRenderDevice = nullptr;
	RootSignatureWrapper* _rootSignatures[(int32)ERootSignatureType::COUNT] = { nullptr, };

public:
	GALRenderDevice* GetOwnerRenderDeivce() const { return _OwnerRenderDevice; }
	const RootSignatureWrapper* GetRootSignature(ERootSignatureType InType) const { return _rootSignatures[(int32)InType]; }

	virtual void InstantiateAllRootSignatures()		= 0;
	virtual void ReleaseAllRoogSignatures()			= 0;
};

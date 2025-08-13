#pragma once
#include "Public/SSGALCommonEnums.h"
#include "SSEngineDefault/Public/SSEngineDefault.h"

class RootSignaturePool;



class RootSignatureWrapper : public INoncopyable
{
protected:
	ERootSignatureType _rootSignatureType = ERootSignatureType::NONE;
	RootSignaturePool* _OwnerRenderSignaturePool = nullptr;

public:
	RootSignatureWrapper(ERootSignatureType rootSignatureType, RootSignaturePool* InOwnerRootSignaturePool);
	virtual ~RootSignatureWrapper();

	virtual bool IsValid() const = 0;
	ERootSignatureType GetRootSignatureType() const { return _rootSignatureType; }
};


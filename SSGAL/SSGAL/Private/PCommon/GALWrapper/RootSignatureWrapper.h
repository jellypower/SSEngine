#pragma once
#include "Public/SSGALCommonEnums.h"

class RootSignaturePool;



class RootSignatureWrapper : public ISSUnknown
{
protected:
	ERootSignatureType _rootSignatureType = ERootSignatureType::NONE;
	RootSignaturePool* _OwnerRenderSignaturePool = nullptr;

public:
	RootSignatureWrapper(ERootSignatureType rootSignatureType, RootSignaturePool* InOwnerRootSignaturePool);

	virtual bool IsValid() const = 0;
	ERootSignatureType GetRootSignatureType() const { return _rootSignatureType; }
};


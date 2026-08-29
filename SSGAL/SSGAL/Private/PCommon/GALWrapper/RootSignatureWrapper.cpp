#include "pch.h"


#include "RootSignatureWrapper.h"

RootSignatureWrapper::RootSignatureWrapper(ERootSignatureType rootSignatureType, RootSignaturePool* InOwnerRootSignaturePool)
	: _rootSignatureType(rootSignatureType), _OwnerRenderSignaturePool(InOwnerRootSignaturePool)
{
}
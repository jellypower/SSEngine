#pragma once
#include "SSGAL/Public/GALRenderDevice/GALRenderDevice.h"


class RootSignaturePool;
class PSOPool;

class PCommonGALRenderDevice : public GALRenderDevice
{
public:
	RootSignaturePool* GetRootSignaturePool() const { return _rootSignaturePool; }
	PSOPool* GetPSOPool() const { return _PSOPool; }

protected:
	RootSignaturePool* _rootSignaturePool = nullptr;
	PSOPool* _PSOPool = nullptr;

};

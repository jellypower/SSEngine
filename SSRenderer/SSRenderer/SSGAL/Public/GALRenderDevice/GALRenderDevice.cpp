#include "GALRenderDevice.h"
#include "SSGAL/Public/GraphicsResourcePool/ConstantBufferPoolManager.h"
#include "SSGAL/Public/GraphicsResourcePool/GraphicsResourceWrapper/ConstantBuffer/ConstantBufferPool.h"

GALRenderDevice::GALRenderDevice(SSRenderer* OwnerRenderer)
	: _OwnerRenderer(OwnerRenderer)
{
}

GALRenderDevice::~GALRenderDevice()
{

}

void GALRenderDevice::TEMP_InitializePSOInstances()
{

}
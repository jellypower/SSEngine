#include "BasicRenderInstance.h"

void BasicRenderInstance::ReleaseGPURenderInstance()
{
	if (_GPUMetadata != nullptr)
	{
		delete _GPUMetadata;
		_GPUMetadata = nullptr;
	}
}

BasicRenderInstance::~BasicRenderInstance()
{
}

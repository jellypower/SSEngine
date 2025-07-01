#include "BasicRenderInstance.h"

#include "SSGAL/Public/GALRenderInstance/GALRIMetadata.h"

void BasicRenderInstance::ReleaseGALRI()
{
	if (_GALRIMetadata != nullptr)
	{
		delete _GALRIMetadata;
		_GALRIMetadata = nullptr;
	}
}

BasicRenderInstance::~BasicRenderInstance()
{
}

#pragma once
#include "SSEngineDefault/Public/RawProfiler/IFrameInfoProcessor.h"


class FrameInfoProcessorBase : public IFrameInfoProcessor
{
	virtual void BeginFrameXXX() override;
	virtual void PerFrameXXX() override;
	virtual void ProcessWindowResizeXXX(uint32 width, uint32 height) override;
};


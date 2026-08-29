#pragma once
#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"


class SSRawInputProcessorBase : public IRawInputProcessor
{
public:
	SSRawInputProcessorBase();
	virtual void Release() override;

public:
	virtual Vector2f GetMouseDelta() const override;

public:
	virtual void ResetCurInputState() override;

	virtual void ProcessInputEndOfFrame() override;

};

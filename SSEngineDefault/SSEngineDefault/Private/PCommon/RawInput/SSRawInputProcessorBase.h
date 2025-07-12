#pragma once
#include "SSEngineDefault/Public/RawInput/IRawInputProcessor.h"


class SSRawInputProcessorBase : public IRawInputProcessor
{
public:
	SSRawInputProcessorBase();


	virtual void ResetCurInputState() override;
	virtual void ProcessInputEndOfFrame() override;
};

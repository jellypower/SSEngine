#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"


struct PoseSlot;

class IAnimWorkee : INoncopyable
{
public:
	virtual const PoseSlot& GetResultPose() const = 0;
	virtual uint64 GetLastUpdateFrame() const = 0;
	virtual bool ShouldUpdateAnim() const = 0;
	

	virtual void UpdateAnimation(float DeltaTime) = 0;
};
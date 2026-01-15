#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"

#include "SSEngineDefault/Public/SSContainer/HashMap.h"



struct PoseSlot
{
	static constexpr int POSE_INLINE_CAPACITY = 200;

	SS::SHasherW PoseName;
	SS::PooledList<SS::SHasherW> BoneNames;
	SS::PooledList<int32> ParentBoneIdx;
	SS::PooledList<Transform> BoneTransforms;
};

class IAnimWorkee : INoncopyable
{
public:
	virtual const PoseSlot& GetResultPose() const = 0;
	virtual uint64 GetLastUpdateFrame() const = 0;
	virtual bool ShouldUpdateAnim() const = 0;
	

	virtual void UpdateAnimation(float DeltaTime) = 0;
};
#pragma once
#include "SSEngineDefault/Public/SSEngineDefault.h"


struct PoseSlot
{
	SS::SHasherW PoseName;
	SS::PooledList<SS::SHasherW> BoneNames;
	SS::PooledList<int32> ParentBoneIdx;
	SS::PooledList<Transform> BoneTransforms;
};

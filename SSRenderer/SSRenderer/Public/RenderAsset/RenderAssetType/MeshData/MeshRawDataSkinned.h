#pragma once
#include "MeshDataDefault.h"


struct alignas(16) BonePlacement
{
	SS::SHasherW BoneName;
	Transform BoneTransform;
};

class MeshRawDataSkinned : public MeshRawDataDefault
{
public:
	MeshRawDataBoneHeader _BoneHeader;

	SS::PooledList<BonePlacement> _BonePlacements;
	

public:
	virtual void ReleaseData() override
	{
		_BonePlacements.FlushMem();
		__super::ReleaseData();
	}
};

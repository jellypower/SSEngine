#pragma once
#include "MeshDataDefault.h"



class MeshRawDataSkinned : public MeshRawDataDefault
{
public:
	MeshRawDataBoneHeader _BoneHeader;

	SS::PooledList<SS::SHasherW> _BoneNames;
	SS::PooledList<Transform> _BonePlacements;

public:
	virtual EMeshType GetMeshType() const override
	{
		return EMeshType::Skinned;
	}

	virtual void ReleaseData() override
	{
		_BonePlacements.FlushMem();
		__super::ReleaseData();
	}
};

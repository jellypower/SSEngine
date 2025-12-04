#pragma once
#include "MeshDataDefault.h"
#include "SSRenderer/Public/RenderCommon/SSBoneType.h"


class MeshRawDataSkinned : public MeshRawDataDefault
{
public:
	SS::PooledList<BonePlacement> _BoneOriginalPose;
};

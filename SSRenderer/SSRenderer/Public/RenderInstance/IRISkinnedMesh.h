#pragma once
#include "IRIMesh.h"

#include "SSGAL/Public/GALStructuredBufferAccessorTypes/SBASkinningJointMatrix.h"

class IRISkinnedMesh : public IRIMesh
{
public:
	virtual const SS::PooledList<SBASkinningJointMatrix>& GetSkeletonPose() const = 0;
	virtual void UpdateSkeletonPose(int32 BoneIdx, const XMMATRIX& WMatrix, const XMMATRIX& RotMatrix) = 0;
};
	
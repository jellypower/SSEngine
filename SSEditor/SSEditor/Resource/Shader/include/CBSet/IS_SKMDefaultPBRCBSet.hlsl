// StaticMeshToDefaultPSInput
#include "include/Types/IS_DefaultTypes.hlsl"


StructuredBuffer<SkinningJoint> SkeletonJointInverse : register(t5);
StructuredBuffer<SkinningJoint> CurrentJoint : register(t6);
// StaticMeshToDefaultPSInput
#include "include/Types/IS_DefaultTypes.hlsl"


StructuredBuffer<SkinningJoint> CurrentJoint : register(t10); // 현재 Pose에 대한 JointMatrix
StructuredBuffer<SkinningJoint> SkeletonJointInverse : register(t11); // 원본 Pose에 대한 InverseJointMatrix
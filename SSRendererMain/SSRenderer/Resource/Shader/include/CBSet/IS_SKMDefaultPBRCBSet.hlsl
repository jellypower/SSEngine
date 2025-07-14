// StaticMeshToDefaultPSInput
#include "include/Types/IS_DefaultTypes.hlsl"


StructuredBuffer<Joint> SkeletonJointInverse : register(t5);
StructuredBuffer<Joint> CurrentJoint : register(t6);
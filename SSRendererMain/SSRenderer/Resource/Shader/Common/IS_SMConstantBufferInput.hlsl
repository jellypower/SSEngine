// StaticMeshToDefaultPSInput
#include "Common/IS_DefaultTypes.hlsl"



cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
    ObjectIDi64 Id;
};

cbuffer GlobalRenderParam : register(b1)
{
    matrix VPMatrix;
    float4 SunDirection;
    float4 SunIntensity;
    float4 ViewerPos;
};
#include "include/Types/IS_DefaultTypes.hlsl"

cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
    ObjectIDi64 Id;
};

cbuffer RenderEnvParam : register(b1)
{
    matrix VPMatrix;
    float4 ViewerPos;
};


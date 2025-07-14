// StaticMeshToDefaultPSInput
#include "include/Types/IS_DefaultTypes.hlsl"



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


cbuffer MaterialParam : register(b2)
{
    float4 baseColorFactor;
    float4 emissiveFactor;
    float normalTextureScale;
    float metallicFactor;
    float roughnessFactor;
};

Texture2D txBaseColor : register(t0);
Texture2D<float3> txNormal : register(t1);
Texture2D<float> txMetallic : register(t2);
Texture2D txEmissive : register(t3);
Texture2D<float> txOcclusion : register(t4);
#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"

sampler g_SamCubeMap : register(s1);


TextureCube<float4> g_CubeTexture : register(t6);

float4 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    float3 viewDir = ViewerPos - input.WorldPos;
    
    float3 CubeSample = g_CubeTexture.Sample(g_SamCubeMap, viewDir);
    return float4(CubeSample, 1);
}
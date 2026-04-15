#include "include/CBSet/IS_DefaultMeshCBSet.hlsl"

sampler g_SamCubeMap : register(s0);


TextureCube<float4> g_CubeTexture : register(t0);

float4 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    float3 viewDir = ViewerPos - input.WorldPos;
    viewDir.y = -viewDir.y;
    float3 CubeSample = g_CubeTexture.Sample(g_SamCubeMap, viewDir);
    return float4(CubeSample, 1);
}
#include "include/Types/IS_DefaultTypes.hlsl"

Texture2D<float3> g_TxNormal : register(t0);
Texture2D<float3> g_TxAlbedo : register(t1);
Texture2D<float> g_TxWorldPos : register(t2);
Texture2D g_TxMetallicRoughness : register(t3);
Texture2D<float> g_TxEmissive : register(t4);


SamplerState samLinear : register(s0);

float4 Main(VS_OUTPUT_FULLSCREEN_QUAD input) : SV_Target
{
    float3 Color = 
        g_TxNormal.Sample(samLinear, input.uv);
    
    return float4(Color, 1);
}
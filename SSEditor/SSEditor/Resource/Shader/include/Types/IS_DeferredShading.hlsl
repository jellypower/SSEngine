#ifndef IS_DEFERREDSHADING_HLSL
#define IS_DEFERREDSHADING_HLSL

struct MRT_Deferred
{
    float3 Normal : SV_Target0;
    float3 Albedo : SV_Target1;
    float3 WorldPos : SV_Target2;
    float2 MetallicRoughness : SV_Target3;
    float3 Emissive : SV_Target4;
    int2 Id : SV_Target5;
};

#endif
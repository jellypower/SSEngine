#ifndef IS_DEFERREDSHADING_HLSL
#define IS_DEFERREDSHADING_HLSL

struct MRT_Deferred
{
    float3 TEMP_FinalColor : SV_Target0;
    float3 Normal : SV_Target1;
    float3 Albedo : SV_Target2;
    float3 WorldPos : SV_Target3;
    float2 MetallicRoughness : SV_Target4;
    float3 Emissive : SV_Target5;
    int2 Id : SV_Target6;
};

#endif
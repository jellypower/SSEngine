#ifndef IS_DEFERREDSHADING_HLSL
#define IS_DEFERREDSHADING_HLSL

struct MRT_Deferred
{
    float3 Color : SV_Target0;
    int2 Id : SV_Target1;
};

#endif
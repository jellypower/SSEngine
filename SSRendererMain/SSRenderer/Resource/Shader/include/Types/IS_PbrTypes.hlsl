#ifndef IS_PBRTYPES_HLSL
#define IS_PBRTYPES_HLSL

struct SurfaceProperties
{
    float3 N;
    float3 V;
    float3 c_diff;
    float3 c_spec;
    float roughness;
    float alpha; // roughness squared
    float alphaSqr; // alpha squared
    float NdotV;
};

#endif
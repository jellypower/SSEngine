#ifndef IS_PBRTYPES_HLSL
#define IS_PBRTYPES_HLSL

struct SurfaceProperties
{
    float3 N;
    float3 V;
    float3 c_diff;
    float3 c_spec;
    float3 baseColor;
    float metallic;
    float roughness;
    float alpha; // roughness squared
    float alphaSqr; // alpha squared
    float NdotV;
};
//    SurfaceProperties surface;
//    surface.N = ComputeNormal(input, txNormal, samLinear, normalTextureScale);
//    surface.V = normalize(ViewerPos - input.WorldPos);
//    surface.NdotV = saturate(dot(surface.N, surface.V));
//    surface.c_diff = baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
//    surface.c_spec = lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
//    surface.baseColor = baseColor;
//    surface.metallic = metallic;
//    surface.roughness = roughness;
//    surface.alpha = roughness * roughness;
//    surface.alphaSqr = surface.alpha * surface.alpha;


struct GBufferProperties
{
    float3 N;
    float3 BaseColor;
    float3 WorldPos;
    float Metallic;
    float Roughness;
    float3 Emissive;
};

#endif
#ifndef IS_PBRLIGHT_HLSL
#define IS_PBRLIGHT_HLSL
#include "include/Types/IS_Constants.hlsl"
#include "include/Types/IS_PbrTypes.hlsl"


float Pow5(float x)
{
    float xSq = x * x;
    return xSq * xSq * x;
}

float3 Fresnel_Shlick(float3 F0, float3 F90, float cosine)
{
    return lerp(F0, F90, Pow5(1.0 - cosine));
}

float Fresnel_Shlick(float F0, float F90, float cosine)
{
    return lerp(F0, F90, Pow5(1.0 - cosine));
}

float Specular_D_GGX(SurfaceProperties Surface, float3 L)
{
    float3 H = normalize(Surface.V + L);
    float NdotH = dot(Surface.N, H);

    float lower = lerp(1, Surface.alphaSqr, NdotH * NdotH);
    return Surface.alphaSqr / max(1e-6, PI * lower * lower);
}

float G_Schlick_Smith(SurfaceProperties Surface, float3 L)
{
    float NdotL = dot(Surface.N, L);
    return 1.0 / max(1e-6, lerp(Surface.NdotV, 1, Surface.alpha * 0.5) * lerp(NdotL, 1, Surface.alpha * 0.5));
}


float3 ComputeDiffuse(SurfaceProperties surface)
{
    float LdotH = saturate(dot(surface.N, normalize(surface.N + surface.V)));
    float fd90 = 0.5 + 2.0 * surface.roughness * LdotH * LdotH;
    float3 DiffuseBurley = surface.c_diff * Fresnel_Shlick(1, fd90, surface.NdotV);

    return DiffuseBurley;
}

float3 ComputeSpecular(SurfaceProperties surface)
{
    float G_V = surface.NdotV + sqrt((surface.NdotV - surface.NdotV * surface.alphaSqr) * surface.NdotV + surface.alphaSqr);
    float3 specular = Fresnel_Shlick(surface.c_spec, 1, surface.NdotV);
    
    return specular;

}



float3 ComputeNormal(
    PS_INPUT_DEFAULT psInput,
    Texture2D<float3> InTxNormal,
    SamplerState InSampler,
    float normalTextureScale)
{
    float3 normal = normalize(psInput.Normal);
    float3 tangent = normalize(psInput.Tangent.xyz);
    float3 bitangent = normalize(cross(normal, tangent)); //* psInput.Tangent.w;

    float3x3 tangentFrame = float3x3(tangent, bitangent, normal);
    
    normal = InTxNormal.Sample(InSampler, psInput.UV0) * 2.0 - 1.0;
    normal = normalize(normal);
    normal = normalize(normal * float3(normalTextureScale, normalTextureScale, 1));

    return mul(normal, tangentFrame);
}

float3 ComputeLightWithCookTorrence(
    SurfaceProperties surface,
    float3 LightDir,
    float3 LightColor)
{
    float3 H = normalize(surface.V + LightDir);
    float VdotH = dot(surface.V, H);
    float F = Fresnel_Shlick(surface.metallic, 1, VdotH); // 우선 프레넬 이펙트 값을 스페큘러 값으로 지정
    float k_d = 1 - F;

    // float3 lambert = baseColor;

    float NdotL = saturate(dot(surface.N, LightDir));

    float cookTorrenceNumerator = Specular_D_GGX(surface, LightDir) * G_Schlick_Smith(surface, LightDir) * F;
    float cookTorrenceDenominator = 4.0 * surface.NdotV * NdotL;
    cookTorrenceDenominator = max(cookTorrenceDenominator, 0.000001);
    
    float Specular = min(F, cookTorrenceNumerator / cookTorrenceDenominator); // Specular == Diffuse
    float DiffuseColor = k_d * surface.baseColor;

    float3 BRDF = DiffuseColor + Specular;
    return BRDF * LightColor * NdotL;
}

#endif
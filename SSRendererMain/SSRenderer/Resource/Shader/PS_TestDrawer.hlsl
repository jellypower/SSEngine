#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"
#include "include/Func/IS_PbrLight.hlsl"


float4 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    float4 baseColor = baseColorFactor * txBaseColor.Sample(samLinear, input.UV0);
    float metallic = metallicFactor * txMetallic.Sample(samLinear, input.UV0);
    float roughness = roughnessFactor;
    float occlusion = txOcclusion.Sample(samLinear, input.UV0);

    float4 emissiveSample = txEmissive.Sample(samLinear, input.UV0);
    float3 emissive = emissiveFactor * emissiveSample.rgb * emissiveSample.a;

    float3 L = normalize(SunDirection);


    SurfaceProperties surface;
    surface.N = ComputeNormal(input, normalTextureScale);
    surface.V = normalize(ViewerPos - input.WorldPos);
    surface.NdotV = saturate(dot(surface.N, surface.V));
    surface.c_diff = baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
    surface.c_spec = lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
    surface.roughness = roughness;
    surface.alpha = roughness * roughness;
    surface.alphaSqr = surface.alpha * surface.alpha;

    float3 colorAccum = emissive;
    
    float3 H = normalize(surface.V + L);
    float VdotH = dot(surface.V, H);
    float k_s = Fresnel_Shlick(metallic, 1, VdotH);
    float k_d = 1 - k_s;

    float3 lambert = baseColor;

    float NdotL = saturate(dot(surface.N, L));

    float cookTorrenceNumerator = Specular_D_GGX(surface, L) * G_Schlick_Smith(surface, L) * k_s;
    float cookTorrenceDenominator = 4.0 * surface.NdotV * NdotL;
    cookTorrenceDenominator = max(cookTorrenceDenominator, 0.000001);
    float cookTorrence = min(k_s, cookTorrenceNumerator / cookTorrenceDenominator);

    float3 BRDF = k_d * lambert + cookTorrence;
    colorAccum += BRDF * NdotL * SunIntensity;
    
    float4 color = float4(colorAccum, baseColor.a);
    return color;
}

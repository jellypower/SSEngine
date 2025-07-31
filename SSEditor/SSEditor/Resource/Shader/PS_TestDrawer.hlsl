#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"
#include "include/Func/IS_PbrFunctionCollection.hlsl"
#include "include/Types/IS_DeferredShading.hlsl"


MRT_Deferred Main(PS_INPUT_DEFAULT input)
{
    MRT_Deferred Output;
    
    float4 baseColor = baseColorFactor * txBaseColor.Sample(samLinear, input.UV0);
    float metallic = metallicFactor * txMetallic.Sample(samLinear, input.UV0);
    float roughness = roughnessFactor;
    float occlusion = txOcclusion.Sample(samLinear, input.UV0);

    float4 emissiveSample = txEmissive.Sample(samLinear, input.UV0);
    float3 emissive = emissiveFactor * emissiveSample.rgb * emissiveSample.a;

    float4 SunDirection = float4(0, 1, 0, 0);
    float4 SunIntensity = float4(1, 1, 1, 1);
    
    float4 MeshShadowPoint = mul(input.WorldPos, ShadowMapVPMat);
    float2 ShadowMapUV = MeshShadowPoint.xy;
    ShadowMapUV.y = -ShadowMapUV.y;
    ShadowMapUV = ShadowMapUV / 2 + float2(0.5, 0.5);
    float ShadowMapDepth = txSingleShadowMap.Sample(samLinear, ShadowMapUV);
    if (ShadowMapDepth < MeshShadowPoint.z - 0.0001)
    {
        SunIntensity = float4(0, 0, 0, 0);
    }
    
    float3 L = normalize(SunDirection);


    SurfaceProperties surface;
    surface.N = ComputeNormal(input, txNormal, samLinear, normalTextureScale);
    surface.V = normalize(ViewerPos - input.WorldPos);
    surface.NdotV = saturate(dot(surface.N, surface.V));
    surface.c_diff = baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
    surface.c_spec = lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
    surface.baseColor = baseColor;
    surface.metallic = metallic;
    surface.roughness = roughness;
    surface.alpha = roughness * roughness;
    surface.alphaSqr = surface.alpha * surface.alpha;

    float3 colorAccum = emissive;
    colorAccum += ComputeLightWithCookTorrence(surface, SunDirection, SunIntensity);
    
    Output.Color = float4(colorAccum, baseColor.a);
    Output.Id = int2(Id.LSB, Id.MSB);
    return Output;
}

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
    
    float3 WorldToViewerPos = normalize(ViewerPos - input.WorldPos);

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

    GBufferProperties Props;
    Props.N = ComputeNormal(input, txNormal, samLinear, normalTextureScale);;
    Props.BaseColor = baseColor;
    Props.WorldPos = input.WorldPos;
    Props.Metallic = metallic;
    Props.Roughness = roughness;
    Props.Emissive = emissive;

    
    float3 colorAccum = emissive;
    
    for (int i = 0; i < DirectionalLightCnt; i++)
    {
        float4 LightDir = DirectionalLights[i].Direction;
        float4 LightIntensity = DirectionalLights[i].Color;
    
        if (i == ShadowMapIdxOnDirectionalLights)
        {
            float4 MeshShadowPoint = mul(input.WorldPos, ShadowMapVPMat);
            float2 ShadowMapUV = MeshShadowPoint.xy;
            ShadowMapUV.y = -ShadowMapUV.y;
            ShadowMapUV = ShadowMapUV / 2 + float2(0.5, 0.5);
            float ShadowMapDepth = txSingleShadowMap.Sample(samLinear, ShadowMapUV);
            
            const float THRESHOLD = 0.0001;
            if (ShadowMapDepth < MeshShadowPoint.z - THRESHOLD)
            {
                LightIntensity *= 0.2;
            }
        }
        
        colorAccum += ComputeLightWithCookTorrence(Props, ViewerPos, LightDir, LightIntensity);
    }
    
    colorAccum += ComputeLightWithCookTorrence(Props, ViewerPos, ViewerPos, AmbientLightIntensity);
    
    colorAccum = saturate(colorAccum);
    Output.Color = float4(colorAccum, baseColor.a);
    Output.Id = int2(Id.LSB, Id.MSB);
    return Output;
}

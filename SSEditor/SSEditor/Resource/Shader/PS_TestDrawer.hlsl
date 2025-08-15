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
    

    GBufferProperties Props;
    Props.N = ComputeNormal(input, txNormal, samLinear, normalTextureScale);;
    Props.BaseColor = baseColor;
    Props.WorldPos = input.WorldPos;
    Props.Metallic = metallic;
    Props.Roughness = roughness;
    Props.Emissive = emissive;
    
    Output.Normal = Props.N;
    Output.Albedo = Props.BaseColor;
    Output.WorldPos = Props.WorldPos;
    Output.MetallicRoughness = float2(Props.Metallic, Props.Roughness);
    Output.Emissive = Props.Emissive;
    Output.Id = int2(Id.LSB, Id.MSB);
    return Output;
}

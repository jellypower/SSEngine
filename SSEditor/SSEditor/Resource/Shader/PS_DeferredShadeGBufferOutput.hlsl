#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"
#include "include/Func/IS_PbrFunctionCollection.hlsl"
#include "include/Types/IS_DeferredShading.hlsl"


MRT_Deferred Main(PS_INPUT_DEFAULT input)
{   
    float4 baseColor = baseColorFactor * txBaseColor.Sample(samLinear, input.UV0);
    float metallic = metallicFactor * txMetallic.Sample(samLinear, input.UV0);
    float roughness = roughnessFactor;
    float occlusion = txOcclusion.Sample(samLinear, input.UV0);

    float4 emissiveSample = txEmissive.Sample(samLinear, input.UV0);
    float3 emissive = emissiveFactor * emissiveSample.rgb * emissiveSample.a;
    

    MRT_Deferred Output;
    Output.Normal = ComputeNormal(input, txNormal, samLinear, normalTextureScale);;
    Output.Albedo = baseColor;
    Output.WorldPos = input.WorldPos;
    Output.MetallicRoughness = float2(metallic, roughness);
    Output.Emissive = emissive;
    Output.Id = int2(Id.LSB, Id.MSB);
    return Output;
}

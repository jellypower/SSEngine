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
    float4 emissive = emissiveFactor * emissiveSample;
    

    MRT_Deferred Output;
    
    float3 f3Normal = ComputeNormal(input, txNormal, samLinear, normalTextureScale);
    float4 f4Normal = float4(f3Normal, 1);
    Output.Normal = f4Normal;
    Output.Albedo = baseColor;
    Output.WorldPos = input.WorldPos;
    Output.MetallicRoughness = float2(metallic, roughness);
    Output.Emissive = emissive;
    Output.Id = int2(Id.LSB, Id.MSB);
    return Output;
}

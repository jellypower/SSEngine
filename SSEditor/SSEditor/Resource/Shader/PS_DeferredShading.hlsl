#include "include/Types/IS_DefaultTypes.hlsl"
#include "include/Func/IS_PbrFunctionCollection.hlsl"


#define DIRECTIONAL_LIGHT_LIMIT 8
#define POINT_LIGHT_LIMIT 32
cbuffer RenderLightParam : register(b0)
{
    int DirectionalLightCnt;
    int PointLightCnt;
    int ShadowMapIdxOnDirectionalLights;
    float4 AmbientLightIntensity;
    matrix ShadowMapVPMat;
    DirectionalLight DirectionalLights[DIRECTIONAL_LIGHT_LIMIT];
    PointLight PointLights[POINT_LIGHT_LIMIT];
};

cbuffer RenderEnvParam : register(b1)
{
    matrix VPMatrix;
    float4 ViewerPos;
};


Texture2D<float3> g_TxNormal : register(t0);
Texture2D<float3> g_TxAlbedo : register(t1);
Texture2D<float3> g_TxWorldPos : register(t2);
Texture2D<float2> g_TxMetallicRoughness : register(t3);
Texture2D<float3> g_TxEmissive : register(t4);

Texture2D<float> g_TxSingleShadowMap : register(t5);


SamplerState samLinear : register(s0);

float4 Main(VS_OUTPUT_FULLSCREEN_QUAD input) : SV_Target
{
    GBufferProperties Props;
    Props.N = g_TxNormal.Sample(samLinear, input.uv);
    Props.BaseColor = g_TxAlbedo.Sample(samLinear, input.uv);
    Props.WorldPos = g_TxWorldPos.Sample(samLinear, input.uv);
    float2 MetallicRoughness = g_TxMetallicRoughness.Sample(samLinear, input.uv);
    Props.Metallic = MetallicRoughness.x;
    Props.Roughness = MetallicRoughness.y;
    Props.Emissive = g_TxEmissive.Sample(samLinear, input.uv);;
    
    float3 WorldToViewerPos = normalize(ViewerPos - Props.WorldPos);
    
    float3 colorAccum = float3(0, 0, 0);
    
    for (int i = 0; i < DirectionalLightCnt; i++)
    {
        float4 LightDir = DirectionalLights[i].Direction;
        float4 LightIntensity = DirectionalLights[i].Color;
    
        if (i == ShadowMapIdxOnDirectionalLights)
        {
            float4 MeshShadowPoint = mul(Props.WorldPos, ShadowMapVPMat);
            float2 ShadowMapUV = MeshShadowPoint.xy;
            ShadowMapUV.y = -ShadowMapUV.y;
            ShadowMapUV = ShadowMapUV / 2 + float2(0.5, 0.5);
            float ShadowMapDepth = g_TxSingleShadowMap.Sample(samLinear, ShadowMapUV);
            
            const float THRESHOLD = 0.0001;
            if (ShadowMapDepth < MeshShadowPoint.z - THRESHOLD)
            {
                LightIntensity *= 0;
            }
        }
        
        colorAccum += ComputeLightWithCookTorrence(Props, WorldToViewerPos, LightDir, LightIntensity);
    }
    
    colorAccum += ComputeLightWithCookTorrence(Props, WorldToViewerPos, WorldToViewerPos, AmbientLightIntensity);
   
    
    colorAccum = saturate(colorAccum);
    colorAccum += Props.Emissive;
    
    return float4(colorAccum, 1);
}
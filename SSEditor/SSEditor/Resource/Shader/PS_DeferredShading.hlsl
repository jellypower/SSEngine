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


SamplerState g_SamLinear : register(s0);
SamplerComparisonState g_SamShadowMapCmp : register(s1);

float4 Main(VS_OUTPUT_FULLSCREEN_QUAD input) : SV_Target
{
    float3 Normal = g_TxNormal.Sample(g_SamLinear, input.uv);
    float NormalSqrLen =
        Normal.x * Normal.x + Normal.y * Normal.y + Normal.z * Normal.z;
    if(NormalSqrLen < 0.1)
    {
        discard;
    }
    
    GBufferProperties Props;
    Props.N = Normal;
    Props.BaseColor = g_TxAlbedo.Sample(g_SamLinear, input.uv);
    Props.WorldPos = g_TxWorldPos.Sample(g_SamLinear, input.uv);
    float2 MetallicRoughness = g_TxMetallicRoughness.Sample(g_SamLinear, input.uv);
    Props.Metallic = MetallicRoughness.x;
    Props.Roughness = MetallicRoughness.y;
    Props.Emissive = g_TxEmissive.Sample(g_SamLinear, input.uv);;
    
    float3 WorldToViewrPos = ViewerPos - Props.WorldPos;
    float WorldToViewrPosDistance = length(WorldToViewrPos);
    float3 WorldToViewerPosDir = WorldToViewrPos / WorldToViewrPosDistance;
    
    float3 colorAccum = float3(0, 0, 0);
    
    for (int i = 0; i < DirectionalLightCnt; i++)
    {
        float4 LightDir = DirectionalLights[i].Direction;
        float4 LightIntensity = DirectionalLights[i].Color;
    
        if (i == ShadowMapIdxOnDirectionalLights)
        {
            float4 f4WorldPos = float4(Props.WorldPos, 1);
            float4 MeshShadowPoint = mul(f4WorldPos, ShadowMapVPMat);

            
            float ShadowProbability = 0;
            
            float2 ShadowMapUV = MeshShadowPoint.xy;
            ShadowMapUV.y = -ShadowMapUV.y;
            ShadowMapUV = ShadowMapUV / 2 + float2(0.5, 0.5);
            
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    const float SHADOW_MAP_PCF_DELTA = 0.0002;
                    float2 ShadowMapUVItem = ShadowMapUV;
                    ShadowMapUVItem.x += (x * SHADOW_MAP_PCF_DELTA);
                    ShadowMapUVItem.y += (y * SHADOW_MAP_PCF_DELTA);
                    
                    const float THRESHOLD = 0.001;
                    float Result = g_TxSingleShadowMap.SampleCmpLevelZero(g_SamShadowMapCmp, ShadowMapUVItem.xy, MeshShadowPoint.z - THRESHOLD);
                    // SampleCmp -> 셰이더는 텍스처를 UV값으로 쓰지만 실제 텍스쳐는 1024 * 1024 같은 사이즈이다.
                    // 그렇기 때문에 하나의 실수에 여러개의 텍스쳐값의 중간이 겹치거나 할 수 있는데
                    // SampleCmp함수는 내가 샘플링한 좌표를 기준으로 주변 텍셀들을 비교해서 값이 샘플러 세팅과 얼마나 유사한지 0 ~ 1사이로 리턴해준다.
                    ShadowProbability += Result;
                }
            }
            
            ShadowProbability /= 9.0f;
            LightIntensity *= ShadowProbability;
        }
        
        colorAccum += ComputeLightWithCookTorrence(Props, WorldToViewerPosDir, LightDir, LightIntensity);
    }
    
    colorAccum += ComputeLightWithCookTorrence(Props, WorldToViewerPosDir, WorldToViewerPosDir, AmbientLightIntensity);
   
    
    colorAccum = saturate(colorAccum);
    colorAccum += Props.Emissive;
    
    return float4(colorAccum, 1);
}
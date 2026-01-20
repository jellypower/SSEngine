// StaticMeshToDefaultPSInput
#include "include/CBSet/IS_DefaultMeshCBSet.hlsl"


cbuffer MaterialParam : register(b2)
{
    float4 baseColorFactor;
    float4 emissiveFactor;
    float normalTextureScale;
    float metallicFactor;
    float roughnessFactor;
};

Texture2D txBaseColor : register(t0);
Texture2D<float3> txNormal : register(t1);
Texture2D<float> txMetallic : register(t2);
Texture2D txEmissive : register(t3);
Texture2D<float> txOcclusion : register(t4);




#define SINGLE_SHADOWMAP_CNT_LIMIT 4
#define DIRECTIONAL_LIGHT_LIMIT 8
#define POINT_LIGHT_LIMIT 32
cbuffer RenderLightParam : register(b3)
{
    int DirectionalLightCnt;
    int PointLightCnt;
    int ShadowMapIdxOnDirectionalLights;
    float4 AmbientLightIntensity;
    matrix ShadowMapVPMat;
    DirectionalLight DirectionalLights[DIRECTIONAL_LIGHT_LIMIT];
    PointLight PointLights[POINT_LIGHT_LIMIT];
};
Texture2D<float> txSingleShadowMap : register(t5);




SamplerState samLinear : register(s0);
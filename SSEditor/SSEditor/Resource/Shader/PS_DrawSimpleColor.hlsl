#include "include/CBSet/IS_DefaultMeshCBSet.hlsl"

cbuffer DrawColorParam : register(b2)
{
    float4 Color;
};


float4 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    return Color;
}

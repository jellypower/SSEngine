// VS_SimpleLineColorToPS
// SimpleLineColorVertex(POSITION + COLOR) -> per-vertex color output
#include "include/CBSet/IS_DefaultMeshCBSet.hlsl"

struct VS_INPUT_SIMPLE_LINE_COLOR
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT_SIMPLE_LINE_COLOR
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_INPUT_SIMPLE_LINE_COLOR Main(VS_INPUT_SIMPLE_LINE_COLOR input)
{
    PS_INPUT_SIMPLE_LINE_COLOR output;
    output.Pos = mul(input.Pos, WMatrix);
    output.Pos = mul(output.Pos, VPMatrix);
    output.Color = input.Color;
    return output;
}

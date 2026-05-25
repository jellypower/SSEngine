// VS_SimpleLineToPS
// SimpleLineVertex(POSITION only) -> PS_INPUT_DEFAULT
#include "include/CBSet/IS_DefaultMeshCBSet.hlsl"

struct VS_INPUT_SIMPLE_LINE
{
    float4 Pos : POSITION;
};

PS_INPUT_DEFAULT Main(VS_INPUT_SIMPLE_LINE input)
{
    PS_INPUT_DEFAULT output = (PS_INPUT_DEFAULT) 0;
    output.Pos = mul(input.Pos, WMatrix);
    output.Pos = mul(output.Pos, VPMatrix);
    return output;
}

// VS_SMToDefaultPSInput
#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"


PS_INPUT_DEFAULT Main(VS_INPUT_DEFAULT input)
{
    PS_INPUT_DEFAULT output = (PS_INPUT_DEFAULT) 0;
    
    output.Pos = mul(input.Pos, WMatrix);
    output.Pos = mul(output.Pos, VPMatrix);
    output.Normal = mul(input.Normal, RotMatrix);
    output.Tangent = mul(input.Tangent, RotMatrix);

    output.UV0 = input.UV0;
    output.UV1 = input.UV1;
    output.WorldPos = mul(input.Pos, WMatrix);
    
    
    return output;
}
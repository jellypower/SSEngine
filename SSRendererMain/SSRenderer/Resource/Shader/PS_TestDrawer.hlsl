#include "Common/IS_SMConstantBufferInput.hlsl"


float4 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    float4 Colors[5] =
    {
        float4(1, 0, 0, 1),
        float4(0, 1, 0, 1),
        float4(0, 0, 1, 1),
        float4(0, 0, 0, 1),
        float4(1, 1, 1, 1)
    };
    int ColorIdx = Id.LSB % 5;

    
    float4 color = Colors[ColorIdx];
    return color;
}

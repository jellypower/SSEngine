
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};


float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = float4(input.Normal, 1);
    return color;
}

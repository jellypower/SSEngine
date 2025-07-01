cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
};

cbuffer GlobalRenderParam : register(b1)
{
    matrix VPMatrix;
    float4 SunDirection;
    float4 SunIntensity;
    float4 ViewerPos;
};


cbuffer MaterialParam : register(b2)
{
    float4 baseColorFactor;
    float4 emissiveFactor;
    float normalTextureScale;
    float metallicFactor;
    float roughnessFactor;
};


const static float4x4 IDENTITY_MATRIX =
{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

const static float4x4 ZERO_MATRIX =
{
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
};

const static int INT_MAX = 2147483647;
const static unsigned int INVALID_IDX = -1;


struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    output.Pos = mul(input.Pos, WMatrix);
    output.Pos = mul(output.Pos, VPMatrix);
    output.Normal = input.Normal;
    output.Tangent = mul(input.Tangent, RotMatrix);

    output.UV0 = input.UV0;
    output.UV1 = input.UV1;
    output.WorldPos = mul(input.Pos, WMatrix);
    
    
    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = float4(input.Normal, 1);
    return color;
}

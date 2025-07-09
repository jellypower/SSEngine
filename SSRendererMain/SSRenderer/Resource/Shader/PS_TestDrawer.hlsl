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




struct ObjectIDi64
{
    int LSB;
    int MSB;
}; // LittleEndian이기 떄문에 LSB가 먼저 나오는게 맞음


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};


cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
    ObjectIDi64 Id;
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



float4 Main(PS_INPUT input) : SV_Target
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

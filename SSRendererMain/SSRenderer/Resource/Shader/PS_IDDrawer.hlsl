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


int2 Main(PS_INPUT input) : SV_Target
{
    int2 Result = int2(Id.LSB, Id.MSB);
    return Result;
}

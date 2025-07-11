#ifndef IS_DEFAULTTYPES_HLSL
#define IS_DEFAULTTYPES_HLSL

struct ObjectIDi64
{
    int LSB;
    int MSB;
};


struct VS_INPUT_DEFAULT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
};

struct PS_INPUT_DEFAULT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
};

#endif
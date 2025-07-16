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

struct VS_INPUT_SKINNED
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    uint4 jointIndices : BLENDINDICES;
    float4 jointWeights : BLENDWEIGHT;
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


struct Joint
{
    float4x4 PosMatrix;
    float4x4 RotMatrix; // Inverse-transpose of PosMatrix
};

#endif
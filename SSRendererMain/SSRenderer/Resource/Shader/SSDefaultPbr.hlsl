cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
    int ObjIDMSB, ObjIDLSB;
};

cbuffer RenderEnvParam : register(b1)
{
    matrix VPMatrix;
    float3 SunDirection;
    float3 SunIntensity;
    float3 ViewerPos;
};


cbuffer MaterialParam : register(b2)
{
    float4 baseColorFactor;
    float4 emissiveFactor;
    float normalTextureScale;
    float metallicFactor;
    float roughnessFactor;
};


#ifdef ENABLE_SKINNING
struct Joint
{
    float4x4 PosMatrix;
    float4x4 RotMatrix; // Inverse-transpose of PosMatrix
};


StructuredBuffer<Joint> SkeletonJointInverse : register(t3);
StructuredBuffer<Joint> CurrentJoint : register(t4);
#endif

//--------------------------------------------------------------------------------------


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


//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
#ifdef ENABLE_SKINNING
    uint4 jointIndices : BLENDINDICES;
    float4 jointWeights : BLENDWEIGHT;
#endif
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

#ifdef ENABLE_SKINNING
    
    int i = 0;
    
    float4 PosAcc = float4(0,0,0,0);
    for (i = 0; i < 4; i++)
    {
        if (input.jointIndices[i] == INVALID_IDX)
        {
            break;
        }
        float4 PosItem = mul(input.Pos, SkeletonJointInverse[input.jointIndices[i]].PosMatrix);
        PosItem = mul(PosItem, CurrentJoint[input.jointIndices[i]].PosMatrix);
        PosAcc += (input.jointWeights[i] * PosItem);
    }
    input.Pos = PosAcc;
    
    
    matrix skinRotMat = ZERO_MATRIX;
    for (i = 0; i < 4; i++)
    {
        if (input.jointIndices[i] == INVALID_IDX)
        {
            break;
        }
        skinRotMat += mul(SkeletonJointInverse[input.jointIndices[i]].RotMatrix, CurrentJoint[input.jointIndices[i]].RotMatrix) * input.jointWeights[i];
    }
    input.Normal = mul(input.Normal, skinRotMat);
    input.Tangent = mul(input.Tangent, skinRotMat);
    
    
#endif
    
    
    
    
    output.Pos = mul(input.Pos, WMatrix);
    output.Pos = mul(output.Pos, VPMatrix);
    output.Normal = mul(input.Normal, RotMatrix);
    output.Tangent = mul(input.Tangent, RotMatrix);

    output.UV0 = input.UV0;
    output.UV1 = input.UV1;
    output.WorldPos = mul(input.Pos, WMatrix);
    
    

    
    return output;
}
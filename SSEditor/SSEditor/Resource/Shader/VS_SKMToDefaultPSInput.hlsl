// VS_SKMToDefaultPSInput
#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"
#include "include/CBSet/IS_SKMDefaultPBRCBSet.hlsl"

#include "include/Types/IS_Constants.hlsl"


PS_INPUT_DEFAULT Main(VS_INPUT_SKINNED input)
{
    PS_INPUT_DEFAULT output = (PS_INPUT_DEFAULT) 0;
    
    
    uint JointIndices[] =
    {
        input.jointIndices0[0],
        input.jointIndices0[1],
        input.jointIndices0[2],
        input.jointIndices0[3],
        input.jointIndices1[0],
        input.jointIndices1[1],
        input.jointIndices1[2],
        input.jointIndices1[3],
    };
    
    float JointWeights[] =
    {
        input.jointWeights0[0],
        input.jointWeights0[1],
        input.jointWeights0[2],
        input.jointWeights0[3],
        input.jointWeights1[0],
        input.jointWeights1[1],
        input.jointWeights1[2],
        input.jointWeights1[3],
    };
    
    int i = 0;
    
    float4 PosAcc = float4(0, 0, 0, 0);
    for (i = 0; i < 8; i++)
    {
        if (JointIndices[i] == INVALID_IDX)
        {
            break;
        }
        float4 PosItem = mul(input.Pos, SkeletonJointInverse[JointIndices[i]].WMatrix);
        PosItem = mul(PosItem, CurrentJoint[JointIndices[i]].WMatrix);
        PosAcc += (JointWeights[i] * PosItem);
    }
    input.Pos = PosAcc;
    
    
    matrix skinRotMat = ZERO_MATRIX;
    for (i = 0; i < 8; i++)
    {
        if (JointIndices[i] == INVALID_IDX)
        {
            break;
        }
        skinRotMat += mul(SkeletonJointInverse[JointIndices[i]].RotMatrix, CurrentJoint[JointIndices[i]].RotMatrix) * JointWeights[i];
    }
    input.Normal = mul(input.Normal, skinRotMat);
    input.Tangent = mul(input.Tangent, skinRotMat);
    
    
    output.Pos = mul(input.Pos, VPMatrix);
    output.Normal = mul(input.Normal, RotMatrix);
    output.Tangent = mul(input.Tangent, RotMatrix);

    output.UV0 = input.UV0;
    output.UV1 = input.UV1;
    output.WorldPos = input.Pos;
    
    
    return output;
}
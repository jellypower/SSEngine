#include "include/CBSet/IS_DefaultPbrCBSet.hlsl"

int2 Main(PS_INPUT_DEFAULT input) : SV_Target
{
    int2 Result = int2(Id.LSB, Id.MSB);
    return Result;
}

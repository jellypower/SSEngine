#include "include/Types/IS_DefaultTypes.hlsl"

VS_OUTPUT_FULLSCREEN_QUAD Main(uint id : SV_VertexID)
{
    VS_OUTPUT_FULLSCREEN_QUAD output;
    
    const float2 positions[8] =
    {
        float2(-1.0, 1.0),
        float2(3.0, 1.0),
        float2(-1.0, -3.0),
    };
    
    

    output.pos = float4(positions[id], 0.0, 1.0);
    output.uv = uvs[id];
    return output;
}
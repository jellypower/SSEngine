#include "include/Types/IS_DefaultTypes.hlsl"

VS_OUTPUT_FULLSCREEN_QUAD Main(uint id : SV_VertexID)
{
    VS_OUTPUT_FULLSCREEN_QUAD output;

    // 3개의 정점으로 전체 화면을 덮는 삼각형 구성
    float2 positions[3] =
    {
        float2(-1.0, 1.0),
        float2(3.0, 1.0),
        float2(-1.0, -3.0),
    };

    float2 uvs[3] =
    {
        float2(0.0, 0.0),
        float2(2.0, 0.0),
        float2(0.0, 2.0),
    };

    output.pos = float4(positions[id], 0.0, 1.0);
    output.uv = uvs[id];
    return output;
}
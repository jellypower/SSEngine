// PS_DrawSimpleColorFromVertex
// per-vertex color passthrough (no color CB)
struct PS_INPUT_SIMPLE_LINE_COLOR
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

float4 Main(PS_INPUT_SIMPLE_LINE_COLOR input) : SV_Target
{
    return input.Color;
}

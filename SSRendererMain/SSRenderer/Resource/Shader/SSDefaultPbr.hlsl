
Texture2D txBaseColor : register(t0);
Texture2D<float3> txNormal : register(t1);
Texture2D<float> txMetallic : register(t2);
Texture2D txEmissive : register(t3);
Texture2D<float> txOcclusion : register(t4);


SamplerState samLinear : register(s0);

cbuffer ModelBuffer : register(b0)
{
    matrix WMatrix;
    matrix RotMatrix;
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


// =====================================================================================================
struct SurfaceProperties
{
    float3 N;
    float3 V;
    float3 c_diff;
    float3 c_spec;
    float roughness;
    float alpha; // roughness squared
    float alphaSqr; // alpha squared
    float NdotV;
};

static const float PI = 3.14159265;
static const float3 kDielectricSpecular = float3(0.04, 0.04, 0.04);
// 유전체(dielectric material)는 전기장 안에서 극성을 지니게 되는 절연체이다.

float3 ComputeNormal(PS_INPUT psInput)
{
    float3 normal = normalize(psInput.Normal);
    float3 tangent = normalize(psInput.Tangent.xyz);
    float3 bitangent = normalize(cross(normal, tangent)); //* psInput.Tangent.w;

    float3x3 tangentFrame = float3x3(tangent, bitangent, normal);
    
    normal = txNormal.Sample(samLinear, psInput.UV0) * 2.0 - 1.0;
    normal = normalize(normal);
    normal = normalize(normal * float3(normalTextureScale, normalTextureScale, 1));

    return mul(normal, tangentFrame);
}

float Pow5(float x)
{
    float xSq = x * x;
    return xSq * xSq * x;
}

// Shlick's approximation of Fresnel
float3 Fresnel_Shlick(float3 F0, float3 F90, float cosine)
{
    return lerp(F0, F90, Pow5(1.0 - cosine));
}

float Fresnel_Shlick(float F0, float F90, float cosine)
{
    return lerp(F0, F90, Pow5(1.0 - cosine));
}

// GGX specular D (normal distribution)
float Specular_D_GGX(SurfaceProperties Surface, float3 L)
{
    float3 H = normalize(Surface.V + L);
    float NdotH = dot(Surface.N, H);

    float lower = lerp(1, Surface.alphaSqr, NdotH * NdotH);
    return Surface.alphaSqr / max(1e-6, PI * lower * lower);
}

// Schlick-Smith specular geometric visibility function
float G_Schlick_Smith(SurfaceProperties Surface, float3 L)
{
    float NdotL = dot(Surface.N, L);
    return 1.0 / max(1e-6, lerp(Surface.NdotV, 1, Surface.alpha * 0.5) * lerp(NdotL, 1, Surface.alpha * 0.5));
}

float3 ComputeDiffuse(SurfaceProperties surface)
{
    float LdotH = saturate(dot(surface.N, normalize(surface.N + surface.V)));
    float fd90 = 0.5 + 2.0 * surface.roughness * LdotH * LdotH;
    float3 DiffuseBurley = surface.c_diff * Fresnel_Shlick(1, fd90, surface.NdotV);

    return DiffuseBurley;
}

float3 ComputeSpecular(SurfaceProperties surface)
{
    float G_V = surface.NdotV + sqrt((surface.NdotV - surface.NdotV * surface.alphaSqr) * surface.NdotV + surface.alphaSqr);
    float3 specular = Fresnel_Shlick(surface.c_spec, 1, surface.NdotV);
    
    return specular;

}

float4 PS(PS_INPUT input) : SV_Target
{
    float4 baseColor = baseColorFactor * txBaseColor.Sample(samLinear, input.UV0);
    float metallic = metallicFactor * txMetallic.Sample(samLinear, input.UV0);
    float roughness = roughnessFactor;
    float occlusion = txOcclusion.Sample(samLinear, input.UV0);

    float4 emissiveSample = txEmissive.Sample(samLinear, input.UV0);
    float3 emissive = emissiveFactor * emissiveSample.rgb * emissiveSample.a;

    float3 L = normalize(SunDirection);


    SurfaceProperties surface;
    surface.N = ComputeNormal(input);
    surface.V = normalize(ViewerPos - input.WorldPos);
    surface.NdotV = saturate(dot(surface.N, surface.V));
    surface.c_diff = baseColor.rgb * (1 - kDielectricSpecular) * (1 - metallic) * occlusion;
    surface.c_spec = lerp(kDielectricSpecular, baseColor.rgb, metallic) * occlusion;
    surface.roughness = roughness;
    surface.alpha = roughness * roughness;
    surface.alphaSqr = surface.alpha * surface.alpha;

    float3 colorAccum = emissive;
    
    float3 H = normalize(surface.V + L);
    float VdotH = dot(surface.V, H);
    float k_s = Fresnel_Shlick(metallic, 1, VdotH);
    float k_d = 1 - k_s;

    float3 lambert = baseColor;

    float NdotL = saturate(dot(surface.N, L));

    float cookTorrenceNumerator = Specular_D_GGX(surface, L) * G_Schlick_Smith(surface, L) * k_s;
    float cookTorrenceDenominator = 4.0 * surface.NdotV * NdotL;
    cookTorrenceDenominator = max(cookTorrenceDenominator, 0.000001);
    float cookTorrence = min(k_s, cookTorrenceNumerator / cookTorrenceDenominator);

    float3 BRDF = k_d * lambert + cookTorrence;
    colorAccum += BRDF * NdotL * SunIntensity;
    
    float4 color = float4(colorAccum, baseColor.a);
    return color;
}

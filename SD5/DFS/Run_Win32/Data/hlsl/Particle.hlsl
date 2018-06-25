struct vertex_in_t
{
    float3 position : POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint4 boneIndices : BONE_INDICES;
    float4 boneWeights : BONE_WEIGHTS;
};

struct vertex_to_fragment_t
{
    float4 position : SV_Position;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;

    float3 world_position : WORLD_POSITION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,


cbuffer matrix_cb : register(b0)
{
    float4x4 MODEL;
    float4x4 VIEW;
    float4x4 PROJECTION;

    float4 EYE_POSITION;
};

cbuffer time_cb : register(b1)
{
    float TIME;

    float WIDTH;

    float HEIGHT;

    float PARM;
};

struct point_light
{
    // PER LIGHT DATA
   // POINT LIGHT, DOT3
    float4 LIGHT_COLOR; // <r, g, b, intensity>
    float4 LIGHT_POSITION;
    float4 LIGHT_DIRECTION;
    float4 ATTENUATION; // <constant, linear, quadratic, padding>
    float4 SPEC_ATTENUATION; // <constant, linear, quadratic, padding>
};

cbuffer light_cb : register(b2)
{
   // GLOBAL LIGHT DATA
    float4 AMBIENT; // <r, g, b, intensity>

    point_light POINT_LIGHTS[8];

   // SURFACE DATA
    float SPEC_FACTOR; // [0, 1] specular scaling factor
    float SPEC_POWER; // [POSITIVE INTEGER]  more speculative power.
    float2 LIGHT_PADDING;
}

Texture2D<float4> tDiffuse : register(t0);
//Texture2D <float4> tTexture : register(t0);
Texture2D<float4> tNormal : register(t1);
StructuredBuffer<float4x4> tSkinMatrices : register(t3);
SamplerState sSampler : register(s0);

float RangeMap(float value, float inMin, float inMax, float outMin, float outMax)
{
    return ((value - inMin) / (inMax - inMin)) * (outMax - outMin) + outMin;
}

float4 TintColor(float4 currentColor, float4 tintColor)
{
    currentColor.x *= tintColor.x;
    currentColor.y *= tintColor.y;
    currentColor.z *= tintColor.z;
    currentColor.w *= tintColor.w;

    return currentColor;
}

// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{

    vertex_to_fragment_t out_data = (vertex_to_fragment_t) 0;

    //float4x4 m0 = mul(vertex.boneWeights.x, tSkinMatrices[vertex.boneIndices.x]);
    //float4x4 m1 = mul(vertex.boneWeights.y, tSkinMatrices[vertex.boneIndices.y]);
    //float4x4 skin_transform = m0 + m1;

    float4 model_position = float4(vertex.position, 1.0f);
    //float4x4 skin_model = mul(skin_transform, MODEL);
    float4 world_position = mul(model_position, MODEL);
    //float4 world_position = mul(model_position, skin_model);
    float4 view_position = mul(world_position, VIEW);
    float4 clip_position = mul(view_position, PROJECTION);

    out_data.position = clip_position;
    out_data.uv = vertex.uv;
    out_data.color = vertex.color;
    out_data.normal = mul(float4(vertex.normal, 0.f), MODEL).xyz;
    out_data.tangent = mul(float4(vertex.tangent, 0.f), MODEL).xyz;
    out_data.world_position = float4(world_position.xyz, 1.0f);
    return out_data;

}

float GetGray(float3 color)
{
    float3 desaturate = float3(0.3, 0.59, 0.11);
    float g = dot(desaturate, color.xyz);
    return g;
}

// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction(vertex_to_fragment_t data) : SV_Target0
{
       // ?  Sample Color at Texture Coordinate data.uv
    float4 color = tDiffuse.Sample(sSampler, data.uv);

    return color * data.color * color.w;

    
}
struct vertex_in_t
{
    float3 position : POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint4 boneIndices : BONE_INDICES;
    float4 boneWeights : BONE_WEIGHTS;
    uint vertexId : SV_VertexID;
};

struct vertex_to_fragment_t
{
    float4 position : SV_Position;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;

    float3 world_position : WORLD_POSITION;

    uint4 boneIndices : BONE_INDICES;
    float4 boneWeights : BONE_WEIGHTS;
    
    
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

struct BlendShapeParameter
{
    float3 position;
    float3 normal;
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

cbuffer blendShapeChannel : register(b5)
{
    int channelSize;
    int vertexSize;
    int propertyCount;
    int padding;
}

Texture2D<float4> tDiffuse : register(t0);
Texture2D<float4> tNormal : register(t1);
Texture2D<float4> tSpecular : register(t2);
StructuredBuffer<float4x4> tSkinMatrices : register(t3);
StructuredBuffer<BlendShapeParameter> tBlendShapes : register(t4);
StructuredBuffer<float> tBlendWeights : register(t5);
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

    float3 blendPosition = (float3) 0;
    float3 blendNormal = (float3) 0;
    float totalBlendWeight = 0;

    for (int blendChannelIndex; blendChannelIndex < channelSize; blendChannelIndex++)
    {
        float blendWeight = tBlendWeights[blendChannelIndex] * 0.01f;
        totalBlendWeight += blendWeight;
        int posIndex = blendChannelIndex * vertexSize + vertex.vertexId;
        float3 deltaBlendPos = (tBlendShapes[posIndex].position) * blendWeight;
        float3 deltaBlendNormal = (tBlendShapes[posIndex].normal) * blendWeight;
        blendPosition += deltaBlendPos;
        blendNormal += deltaBlendNormal;
    }
    
    if(totalBlendWeight > 1.f)
    {
        blendPosition /= totalBlendWeight;
        blendNormal /= totalBlendWeight;
    }

    /*if (length(blendPosition) > 0.f)
    {
        vertex.color = float4(1, 0, 0, 1);
    }*/

    float w0 = vertex.boneWeights.x;
    float w1 = vertex.boneWeights.y;
    float w2 = vertex.boneWeights.z;
    float w3 = vertex.boneWeights.w;
    float normalizeScale = 1.f;
    if (w0 + w1 + w2 + w3 == 0.f)
    {
        w0 = 1.f;
        w1 = 0.f;
        w2 = 0.f;
        w3 = 0.f;
    }
        
    if (w0 + w1 + w2 + w3 != 1.f && w0 + w1 + w2 + w3 != 0.f)
        normalizeScale = 1.f / (w0 + w1 + w2 + w3);
    w0 *= normalizeScale;
    w1 *= normalizeScale;
    w2 *= normalizeScale;
    w3 *= normalizeScale;

    float4x4 m0 = w0 * tSkinMatrices[vertex.boneIndices.x];
    float4x4 m1 = w1 * tSkinMatrices[vertex.boneIndices.y];
    float4x4 m2 = w2 * tSkinMatrices[vertex.boneIndices.z];
    float4x4 m3 = w3 * tSkinMatrices[vertex.boneIndices.w];
    float4x4 skin_transform = m0 + m1 + m2 + m3;
    skin_transform[3][3] = 1.f;

    float4 local_position = float4(vertex.position + blendPosition, 1.0f);
    //float4 local_position = float4(blendPosition * totalBlendWeight + vertex.position * (1.f - totalBlendWeight), 1.0f);
    float4x4 skin_model = mul(skin_transform, MODEL);
    //float4 world_position = mul(local_position, MODEL);
    float4 world_position = mul(local_position,skin_model);
    float4 view_position = mul(world_position, VIEW);
    float4 clip_position = mul(view_position, PROJECTION);

    out_data.position = clip_position;
    out_data.uv = vertex.uv;
    out_data.color = vertex.color;
    out_data.normal = mul(float4(vertex.normal + blendNormal, 0.f), skin_model).xyz;
    out_data.tangent = mul(float4(vertex.tangent, 0.f), skin_model).xyz;
    out_data.world_position = float4(world_position.xyz, 1.0f);
    out_data.boneIndices = vertex.boneIndices;
    out_data.boneWeights = float4(w0, w1, w2, w3); //vertex.boneWeights;
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
    // UV check
    //return float4(data.uv.x,0.f,0.f, 1.0f);

    float4 color = tDiffuse.Sample(sSampler, data.uv);

    //return color;

    float4 diffuse = color * data.color;

    float3 dataNormal = normalize(data.normal.xyz);
    //dataNormal = normalize(dataNormal);
    //normal += dataNormal;

    float3 tangent = normalize(data.tangent);
    float3 bitangent = normalize(cross(tangent, dataNormal));
    tangent = normalize(cross(dataNormal, bitangent));

    float3x3 tbn =
    {
        tangent,
        bitangent,
        dataNormal
    };

    //tbn = transpose(tbn);

    float3 normal_color = tNormal.Sample(sSampler, data.uv).xyz;
    normal_color = normal_color * float3(2, 2, -2) - float3(1, 1, -1);
    normal_color = normalize(normal_color);

    float3 surfaceNormal = mul(normal_color, tbn);

    surfaceNormal = normalize(surfaceNormal);

    float3 normal = dataNormal;
    
   // calculate the eye vector
    float3 vector_to_eye = EYE_POSITION.xyz - data.world_position;
    float3 eye_vector = -normalize(vector_to_eye); // get direction from eye to fragment

   // light factor

   // first, calculate ambient (just added in - once per fragment)
    float4 ambient_factor = float4(AMBIENT.xyz * AMBIENT.w, 1.0f);

    //return diffuse * ambient_factor;

    float4 light_diffuse_factor = float4(0.f, 0.f, 0.f, 0.f);
    float4 spec_color = float4(0.f, 0.f, 0.f, 0.f);

   // PER LIGHT START--------------------------------------------------------------------------
    

    for (int i = 0; i < 8; i++)
    {
         // point light factor - light factors are calculated PER LIGHT and added 
    // to the ambient factor to get your final diffuse factor
        float3 vector_to_light = POINT_LIGHTS[i].LIGHT_POSITION.xyz - data.world_position;
        float distance_to_light = length(vector_to_light);
        float3 dir_to_light = normalize(vector_to_light); /// distance_to_light;

        float dot3 = saturate(dot(dir_to_light, surfaceNormal));

        if (data.normal.x == 0.f && data.normal.y == 0.f && data.normal.z == 0.f)
            dot3 = 1.f;

        float faceDot3 = dot(dir_to_light, dataNormal);
        //if (faceDot3 < 0.f)
        //    continue;

        float attenuation = POINT_LIGHTS[i].LIGHT_COLOR.w / (POINT_LIGHTS[i].ATTENUATION.x 
      + distance_to_light * POINT_LIGHTS[i].ATTENUATION.y
      + distance_to_light * distance_to_light * POINT_LIGHTS[i].ATTENUATION.z);

   // don't allow attenuation to go above one (so don't allow it to blow out the image)
        attenuation = saturate(attenuation);

        float4 light_color = float4(POINT_LIGHTS[i].LIGHT_COLOR.xyz, 1.0f);
        light_diffuse_factor += saturate(light_color * dot3 * attenuation);

   // Calculate Spec Component
        
        float spec_attenuation = POINT_LIGHTS[i].LIGHT_COLOR.w / (POINT_LIGHTS[i].SPEC_ATTENUATION.x 
      + distance_to_light * POINT_LIGHTS[i].SPEC_ATTENUATION.y
      + distance_to_light * distance_to_light * POINT_LIGHTS[i].SPEC_ATTENUATION.z);
   
   // figure how much the reflected vector coincides with the eye vector
        float3 ref_light_dir = reflect(dir_to_light, surfaceNormal);
        
        float spec_dot3 = saturate(dot(ref_light_dir, eye_vector));

        //if (dot(dir_to_light, surfaceNormal) < 0.f)
        //    spec_dot3 = 0.f;

        if (data.normal.x == 0.f && data.normal.y == 0.f && data.normal.z == 0.f)
            spec_dot3 = 1.f;

        float4 specColor = tDiffuse.Sample(sSampler, data.uv);

   // take it to the spec power, and scale it by the spec factor and our attenuation
        float spec_factor = spec_attenuation * SPEC_FACTOR * pow(spec_dot3, SPEC_POWER);
        spec_factor *= 0.2f;
        spec_color += saturate(light_color * specColor * spec_factor);

    }

  

   // END PER LIGHT PART--------------------------------------------------------------------------

   // calculate final diffuse factor by adding all light factors to the ambiant factor
    float4 diffuse_factor = saturate(ambient_factor + light_diffuse_factor);

   // final color is our diffuse color multiplied by diffuse factor.  Spec color is added on top. 
    float4 final_color = diffuse_factor * diffuse + spec_color;

   // calculate final color
    return final_color;
/*
	//Sample Color at Texture Coordinate data.uv
    //float4 texColor = tTexture.Sample(sSampler, data.uv);
    float4 texColor = tDiffuse.Sample(sSampler, data.uv);

    float4 diffuse = texColor * data.color;
   

    float4 ambientFactor = float4(float3(AMBIENT_FACTOR.xyz * AMBIENT_FACTOR.w), 1.0f);
    ambientFactor *= AMBIENT_FACTOR.w;

    //return texColor * data.color * ambientFactor;

    float3 vectorToLight = LIGHT_POSITION.xyz - data.world_position.xyz;
    float disToLight = length(vectorToLight);
    float3 dirToLight = normalize(vectorToLight);
    float dirDotNor = dot(dirToLight, data.normal);
    float4 dot3Factor = float4(dirDotNor, dirDotNor, dirDotNor, 1.0f);
    float attenuation = LIGHT_COLOR.w / (ATTENUATION.x 
      + disToLight * ATTENUATION.y
      + disToLight * disToLight * ATTENUATION.z);

    float4 lightColor = float4(LIGHT_COLOR.xyz, 1.f);
	
    float4 dirLightFactor = lightColor * dot3Factor * attenuation;
    dirLightFactor = saturate(dirLightFactor);

    float distanceToLight = length(vectorToLight);

    float4 LightFactor = saturate(ambientFactor + dirLightFactor);

    float4 finalColor = texColor * data.color * LightFactor;

    return finalColor;*/
}
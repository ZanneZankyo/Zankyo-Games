struct vertex_in_t
{
	float3 position : POSITION;
	float2 uv : UV;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct vertex_to_fragment_t
{
	float4 position : SV_Position;
	float2 uv : UV;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,

cbuffer matrix_cb : register(b0)
{
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
};


cbuffer time_cb : register(b1)
{
	float TIME;

	float WIDTH;

	float HEIGHT;

	float PARM;
};

cbuffer light_cb : register(b2)
{
	float4 AMBIENT_FACTOR; // (r,g,b,intensity)
	float4 LIGHT_COLOR;
	float4 LIGHT_POSITION;
	float4 LIGHT_DIRECTION;
	float4 ATTENUATION;
}

Texture2D <float4> tTexture : register(t0);
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

	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

	float4 model_position = float4(vertex.position, 1.0f);
	float4 world_position = mul(model_position, MODEL);
	float4 view_position = mul(world_position, VIEW);
	float4 clip_position = mul(view_position, PROJECTION);

	out_data.position = clip_position;
	out_data.uv = vertex.uv;
	out_data.color = vertex.color;
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

	//Sample Color at Texture Coordinate data.uv
	float4 texColor = tTexture.Sample(sSampler, data.uv);

	//for debug
	return texColor;

	float4 ambientFactor = float4(float3(AMBIENT_FACTOR.xyz),1.0f);

	float3 vectorToLight = LIGHT_POSITION.xyz - data.position.xyz;
	float3 dirToLight = normalize(vectorToLight);
	float4 dot3Factor = float4( float3(dot(dirToLight, data.normal)), 1.0f);
	float4 lightColor = float4(LIGHT_COLOR.xyz * LIGHT_COLOR.w, 1.f);
	
	float4 dirLightFactor = dot3Factor*lightColor;
	dirLightFactor = saturate(dirLightFactor);

	float distanceToLight = length(vectorToLight);

	float4 LightFactor = ambientFactor + dirLightFactor;

	float finalColor = LightFactor * texColor;

	return finalColor;
}
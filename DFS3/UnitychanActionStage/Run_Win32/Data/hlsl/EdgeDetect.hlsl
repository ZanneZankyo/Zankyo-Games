struct vertex_in_t
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv : UV;
};

struct vertex_to_fragment_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : UV;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1)
{
	float TIME;

	float WIDTH;

	float HEIGHT;

	float PARM;
};

cbuffer matrix_cb : register(b0)
{
	/*float4x4 MODEL;
	float4x4 VIEW;*/
	float4x4 PROJECTION;
};


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

	float4 transformed_position = mul(float4(vertex.position, 1.f), PROJECTION);

	out_data.position = transformed_position;
	out_data.uv = vertex.uv;
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
	float checkDistance = 0.0001f + PARM * 0.002f;
	int checkSides = 32;
	float deltaDegrees = 360.f / checkSides;

	//Sample Color at Texture Coordinate data.uv
	float4 currentColor = tTexture.Sample(sSampler, data.uv);
	float currentGray = GetGray(currentColor.xyz);

	for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
	{
		float rad = radians(degrees);
		float2 nextUv = data.uv;
		nextUv.x += cos(rad)*checkDistance;
		nextUv.y += sin(rad)*checkDistance;
		float4 nextColor = tTexture.Sample(sSampler, nextUv);
		float nextGray = GetGray(nextColor.xyz);
		if (nextGray - currentGray > 0.08)
			return TintColor(currentColor, data.color);
	}
	return float4(1, 1, 1, 1);
}





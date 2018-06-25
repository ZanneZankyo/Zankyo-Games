struct vertex_in_t
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct vertex_to_fragment_t
{
	float4 position : SV_Position;
	float2 uv : UV;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1)
{
	float TIME;

	float3 PADDING;
};


Texture2D <float4> tTexture : register(t0);
SamplerState sSampler : register(s0);


// VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{
	
	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

	out_data.uv = vertex.uv;
	out_data.position = float4(vertex.position, 1.0f);
	
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
	float checkDistance = 0.002f;
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
		if(nextGray-currentGray > 0.05)
			return float4(0, 0, 0, 1);
	}
	return float4(1, 1, 1, 1);
	//return currentColor.xyzw;
}





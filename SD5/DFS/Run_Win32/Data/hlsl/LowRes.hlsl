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

float GetClosestChuck(float x, int split)
{

	// 0.001 ~ 0.010 = 0.005;
	float unit = 1.f / split;

	x = (int)(x * split);
	x /= (float)split;

	return x + unit * 0.5f;
}


// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{
	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

	out_data.position = float4(vertex.position, 1.0f);
	//out_data.position.x += 1.f - out_data.position.x;
	out_data.uv = vertex.uv;
	return out_data;
}

// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction(vertex_to_fragment_t data) : SV_Target0
{

	// 0.001 ~ 0.010 = 0.005;

	data.uv.x = GetClosestChuck(data.uv.x, 300);
	data.uv.y = GetClosestChuck(data.uv.y, 300);

	//Sample Color at Texture Coordinate data.uv
	float4 color = tTexture.Sample(sSampler, data.uv);



	return color.xyzw;
}







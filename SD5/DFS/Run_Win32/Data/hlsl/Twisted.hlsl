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

	float WIDTH;

	float HEIGHT;

	float PADDING;
};

cbuffer matrix_cb : register(b2)
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


// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{

	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

	float4 transformed_position = mul(float4(vertex.position, 1.f), PROJECTION);

	out_data.position = transformed_position;
	out_data.uv = vertex.uv;
	return out_data;
}

// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction(vertex_to_fragment_t data) : SV_Target0
{
	//Sample Color at Texture Coordinate data.uv

	float waveWidth = 0.2f;

	float distance = sqrt((data.uv.y - 0.5f)*(data.uv.y - 0.5f) + (data.uv.x - 0.5f)*(data.uv.x - 0.5f));
	//distance %= waveWidth;

	if (distance < TIME * 0.05)
	{
		data.uv.y += cos(distance * 100 + TIME * 1) * 0.005f;
		data.uv.x += sin(distance * 100 + TIME * 1) * 0.005f;
	}

	float4 color = tTexture.Sample(sSampler, data.uv);

	return color.xyzw;
}





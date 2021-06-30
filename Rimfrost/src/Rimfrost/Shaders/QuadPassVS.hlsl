
struct vs_in
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

vs_out main(vs_in input)
{
    vs_out output = (vs_out)0;
    output.position = float4(input.position, 1);
    output.uv = input.uv;
	return output;
}
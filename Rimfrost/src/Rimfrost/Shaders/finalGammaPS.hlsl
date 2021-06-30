Texture2D uncorrectedFrame : register(t0);


SamplerState smpler : register(s0);

cbuffer gammaFactor : register(b0)
{
    float gamma;
};

float4 main() : SV_TARGET
{
    
    
    //uncorrectedFrame.Sample(smpler, )
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
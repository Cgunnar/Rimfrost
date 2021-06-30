

cbuffer VP : register(b5)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
};

cbuffer Transforms : register(b1)
{
    matrix worldMatrix;
};

struct vs_in
{
    float3 position_local : POSITION;
    float3 normals : NORMAL;
    float2 textureUV : TEXCOORD;
};

struct vs_out
{
    float4 position_clip : SV_POSITION;
    float4 normals : NORMAL;
    float2 textureUV : TEXCOORD;
    float4 worldPosition : WORLDPOSITION;
};

vs_out main(vs_in input)
{
    vs_out output = (vs_out)0;
    output.textureUV = input.textureUV;
    output.normals = float4(input.normals, 0.0f);
    output.worldPosition = float4(input.position_local, 1.0f);
    output.position_clip = float4(input.position_local, 1.0f);


    output.worldPosition = mul(worldMatrix, output.worldPosition);
    output.normals = mul(worldMatrix, output.normals);


    matrix MVP;
    MVP = mul(viewMatrix, worldMatrix);
    MVP = mul(projectionMatrix, MVP);

    output.position_clip = mul(MVP, output.position_clip);
    return output;
}
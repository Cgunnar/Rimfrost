
Texture2D g_position : register(t0);

Texture2D<int> modelIDs : register(t1);
SamplerState smplr : register(s0);

cbuffer PerFrameBuffer : register(b5)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
    float padding;
    int2 mouseCoord;
};

struct MousePickingStruct
{
    bool picked;
    int id;
    float3 pos;
};

RWStructuredBuffer<MousePickingStruct> mousePicking;

struct ps_in
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

void main(ps_in input)
{       
    if (mouseCoord.x == (int) input.position.x && mouseCoord.y == (int) input.position.y)
    {
        MousePickingStruct ms;
        ms.id = modelIDs.Load(int3(mouseCoord.x, mouseCoord.y, 0));
        ms.pos = g_position.Sample(smplr, input.uv).xyz;
        ms.picked = true;
        
        mousePicking[0] = ms;
    }
}
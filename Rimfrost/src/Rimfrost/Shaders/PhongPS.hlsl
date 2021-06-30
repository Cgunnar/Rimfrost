

#ifdef NORMAL_MAP
    Texture2D normalMap : register(t3);
#endif

#ifdef DIFFUSE_TEXTURE
    Texture2D myTexture : register(t0);
#endif

#ifdef SPECULAR_MAP
    Texture2D specTex : register(t2);
#endif


SamplerState mySamplerState : register(s0);

struct PointLightStruct
{
    float4 pos;
    float3 color;
    float strength;
};


StructuredBuffer<PointLightStruct> lightBuffer : register(t4);

//struct MousePickingStruct
//{
//    bool picked;
//    int id;
//    float3 pos;
//    float3 col;
//    //float pad;
//    //int id;
//    //float3 paddarr;
//};

//RWStructuredBuffer<MousePickingStruct> mousePicking;

cbuffer Transforms : register(b1)
{
    matrix worldMatrix;
};

cbuffer SubModelMisc : register(b2)
{
    int subModelID;
};

cbuffer Material : register(b4)
{
    float3 specularColor;
    float shininess;
    float3 materialColor;
    float opacity;
    float3 emissiveColor;
}

cbuffer PerFrameBuffer : register(b5)
{
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
    float padding;
    int2 mouseCoord;
};

#ifdef NORMAL_MAP
struct ps_in
{
    float4 position_clip : SV_POSITION;
    float4 normals : NORMAL;
    float2 textureUV : TEXCOORD;
    float4 worldPosition : WORLDPOSITION;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
    
};
#else
struct ps_in
{
    float4 position_clip : SV_POSITION;
    float4 normals : NORMAL;
    float2 textureUV : TEXCOORD;
    float4 worldPosition : WORLDPOSITION;
};
#endif

struct ps_out
{
    float4 col : SV_TARGET0;
    float4 pos : SV_TARGET1;
    int id : SV_TARGET2;
};


//float4 main(ps_in input) : SV_TARGET
ps_out main(ps_in input)
{
    float3 ia = float3(0.0f, 0.0f, 0.0f);
    float3 id = float3(0.0f, 0.0f, 0.0f);
    float3 is = float3(0.0f, 0.0f, 0.0f);

    float3 idSum = float3(0, 0, 0);
    float3 isSum = float3(0, 0, 0);

    //set light colors
    float3 la = float3(0.2f, 0.2f, 0.2f);
    //float3 ld = float3(0.8f, 0.8f, 0.8f);
    //float3 ls = float3(1.0f, 1.0f, 1.0f);

    //set material colors
    float3 kd = materialColor;
    float3 ka = materialColor;
    float3 ks = specularColor;
    
    float alpha = opacity;
    
    float3 normal = input.normals.xyz;
    
#ifdef NORMAL_MAP
    float3 normalTanSpace = /*normalize*/(normalMap.Sample(mySamplerState, input.textureUV).xyz); //normalization fucks up sponza and brickwall but fixes crysis
    normalTanSpace = 2 * normalTanSpace - float3(1, 1, 1);
    normalTanSpace = normalize(normalTanSpace);
    
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.biTangent);
    float3 N = normalize(input.normals.xyz);
    
    float3x3 TBN = float3x3(T, B, N);
    TBN = transpose(TBN); //so i can use mul the right way :/
    normal = normalize(mul(TBN, normalTanSpace));
    
#endif
    
#ifdef DIFFUSE_TEXTURE
    float4 texColor = myTexture.Sample(mySamplerState, input.textureUV);
    ka = texColor.xyz;
    kd = texColor.xyz;
    
    alpha = texColor.w;
#endif
    
#ifdef SPECULAR_MAP
    float4 texSpec = specTex.Sample(mySamplerState, input.textureUV);
    ks = texSpec.xyz;
#endif
    
    
    //gamma
    //texColor = pow(texColor, 2.2);
    //texSpec = pow(texSpec, 2.2);
    
    
   



    uint numLights, size;
    lightBuffer.GetDimensions(numLights, size);
    float3 finalColor = float3(0, 0, 0);
    for (uint i = 0; i < numLights; i++)
    {
        float3 dirFromLight = normalize(input.worldPosition.xyz - lightBuffer[i].pos.xyz);
        float3 dirToLight = -dirFromLight;

        float diffuseFactor = saturate(dot(dirToLight, normalize(normal.xyz)));
        if (diffuseFactor > 0.0f)
        {
            //id = ld * kd * diffuseFactor;
            id = lightBuffer[i].color * 0.8 * diffuseFactor;

            float3 r = normalize(reflect(dirFromLight, normal.xyz));
            float3 v = normalize(cameraPos.xyz - input.worldPosition.xyz);

            float dotCheckNonZero = saturate(dot(r, v));
            if (dotCheckNonZero > 0)
            {
                //is = ls * ks * (pow(dotCheckNonZero, 1000.0f));
                is = lightBuffer[i].color * (pow(dotCheckNonZero, shininess));
            }
        }
        float lengt = length(input.worldPosition.xyz - lightBuffer[i].pos.xyz);
        float3 pixelColor = id + is;

        id /= (lengt * lengt);
        is /= (lengt * lengt);

        idSum += id * lightBuffer[i].strength;
        isSum += is * lightBuffer[i].strength;

        //float3 pixelColor = ia + ((id + is) / (1));
        //finalColor += pixelColor;
    }
    ia = la * ka;
    id = saturate(idSum) * kd;
    is = saturate(isSum) * ks;


    finalColor = saturate(ia + id + is);
    
 
    
    
    ps_out output = (ps_out) 0;
    output.pos = input.worldPosition;
    output.id = subModelID;
    output.col = float4(finalColor, alpha);
    return output;
    
    //return float4(finalColor, alpha);
    //return float4(finalColor, 1.0f);
}
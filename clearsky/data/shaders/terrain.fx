#define MATH_E 2.71828

#define MAX_LIGHTS 3
#define MAX_TEXTURE_LAYERS 7

#define LIGHT_TYPE_PARALLEL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light
{	

	float3	pos;
	float	pad;
	float3	dir;
	float	pad2;
	float4	ambient;
	float4	diffuse;
	float4	spec;
	float	att0;
	float	att1;
	float	att2;//float3 att;
	float	spotPower;
	float	range;
	float	falloff;
	float	theta;
	float	phi;
	int		lightType;
	int3	pad3;
	bool	active;
	int3	pad4;
	
};

#define FOG_TYPE_LINEAR 0
#define FOG_TYPE_EXPONENT 1
#define FOG_TYPE_EXPONENT2 2

struct Fog
{
	float4 color;
	int fogType;
	float start;
	float end;
	float density;
};

struct Viewport
{
	int x;
	int y;
	int width;
	int height;
	int minDepth;
	int maxDepth;
	int minHeight;
};

struct Material
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float4 emissive;
};
	
cbuffer cbPerFrame
{	
	int time;
	float3 gEyePosW;
	matrix view;
	matrix projection;
	matrix mvp;
};


cbuffer cbPerObject
{
	matrix world;
	bool heightTextureOn  : register(b0) = false;
};

int gNumTextureLayers = 1;

Texture2D gTexture2D0;
Texture2D gTexture2D1;
Texture2D gHeightTexture2D;

SamplerState WrapSamplerState
{
	Filter = ANISOTROPIC;
	AddressU = wrap;
	AddressV = wrap;
};

SamplerState ClampSamplerState
{
	Filter = ANISOTROPIC;
	AddressU = clamp;
	AddressV = clamp;
};

struct VS_IN
{
	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float3 tangentL : TANGENT;
	float2 texCoord0 : texCoord0;
	float2 texCoord1 : texCoord1;
//	float2 texCoord2 : texCoord2;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION; 
	float3 normalW : NORMAL;
	float3 tangent : TANGENT;
    float3 posW    : POSITION;
	float2 texCoord0 : texCoord0;
	float2 texCoord1 : texCoord1;
//	float2 texCoord2 : texCoord2;
	float4 posCamera : CAMPOS;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	//vIn.posL.xy += 0.5f*sin(vIn.posL.x)*sin(3*fTime);
	//vIn.posL.z *= 0.6f + 0.4f*sin(2*fTime);

	//vIn.color.rgb = 1-vIn.posL.y/255;

	matrix transform = mul(world, view);
	transform = mul(transform, projection);

	vOut.posH = mul(float4(vIn.posL,1.0f), transform);

	float4 worldPos = mul(float4(vIn.posL, 1.0f), world);
	vOut.posCamera  = mul(worldPos, view);

	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), transform);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), transform);
	vOut.tangent = vIn.tangentL;
		
	// Output vertex attributes for interpolation across triangle.
	//vOut.diffuse = vIn.diffuse;
	//vOut.spec    = vIn.spec;
	
	vOut.texCoord0 = vIn.texCoord0;
	vOut.texCoord1 = vIn.texCoord1;
	//vOut.texCoord2 = vIn.texCoord2;

	return vOut;
}
 
float4 PS_LIGHT(VS_OUT pIn) : SV_Target
{
	float4 output = float4(1,1,1,1);//pIn.color;	
	
	if(gNumTextureLayers>0)
		{
			output = output*gTexture2D0.Sample(WrapSamplerState, pIn.texCoord0);

			if(gNumTextureLayers>1)
			{
				output = output*gTexture2D1.Sample(WrapSamplerState, pIn.texCoord1);

			}//end layer 1
		}//end layer 0

	//output = output*gTexture2D0.Sample( DefaultSamplerState, pIn.texCoord0);
	
    return output;//output*pIn.color;
}


technique10 Default
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_LIGHT() ) );
	}
}

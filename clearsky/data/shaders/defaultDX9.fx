#define MATH_E 2.71828

#define MAX_LIGHTS 6
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


struct SurfaceInfo
{
	float3 pos;
    float3 normal;
    float4 diffuse;
    float4 spec;
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
	float time;
	float3 gEyePosW;
	matrix view;
	matrix projection;
};


cbuffer cbPerObject
{
	Material gMaterial;
	matrix world;
};

Texture2D gDiffuseMap;
Texture2D gAlphaMap;

Texture2D gTexture2D0;
Texture2D gTexture2D1;
Texture2D gTexture2D2;

int gNumTextureLayers = 1;

Light gLight[MAX_LIGHTS];

//cbuffer cbRare
//{
	Viewport gViewport;
	Fog gFog;
	bool gNormalsPosition = false;
	bool lightOn = false;
	bool fogOn = true;
	bool textureOn = false;
	bool alphaMapOn = false;
//};

bool textureWrap = true;



//disable depthtest in screenspace
DepthStencilState ScreenspaceDepth
{
	DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

struct VS_IN
{
	float3 posL     : POSITION;
	float3 normalL  : NORMAL;
	float4 color    : COLOR;
	float2 texCoord0 : texCoord0;
	float2 texCoord1 : texCoord1;
	float2 texCoord2 : texCoord2;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION; 
	float3 normalW : texCoord4;
	float4 color    : COLOR;
    float3 posW    : texCoord0;
	float2 texCoord0 : texCoord1;
	float2 texCoord1 : texCoord2;
	float2 texCoord2 : texCoord3;
	float4 posCamera : COLOR1;
};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	matrix transform = mul(world, view);
	transform = mul(transform, projection);

	vOut.posH = mul(float4(vIn.posL,1.0f), transform);

	float4 worldPos = mul(float4(vIn.posL, 1.0f), world);
	vOut.posCamera  = mul(worldPos, view);

	// Transform to world space space.
	vOut.posW    = (float3)mul(float4((float3)vIn.posL, 1.0f), transform);
	vOut.normalW = (float3)mul(float4((float3)vIn.normalL, 0.0f), transform);
			
	vOut.color = vIn.color;

	vOut.texCoord0 = vIn.texCoord0;
	vOut.texCoord1 = vIn.texCoord1;
	vOut.texCoord2 = vIn.texCoord2;

	return vOut;
}
 
float4 PS_LIGHT(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;


    return output;//output*pIn.color;
}

VS_OUT VS_ScreenSpace(VS_IN vIn)
{
	VS_OUT vOut;
	
	vOut.posH.x = (vIn.posL.x / (gViewport.width/2.0))-1;
	vOut.posH.y = -(vIn.posL.y / (gViewport.height/2.0))+1;
	vOut.posH.z = vIn.posL.z;
	vOut.posH.w = 1;

	vOut.posCamera  = float4(0,0,0,0);
		
	vIn.normalL = vIn.posL;

	vOut.posW    = (float3)float4(vIn.posL, 1.0f);
	vOut.normalW = (float3)float4(vIn.normalL, 0.0f);
			
	vOut.color = vIn.color;
		
	//Output vertex attributes for interpolation across triangle.
	//vOut.diffuse = vIn.diffuse;
	//vOut.spec    = vIn.spec;
	
	vOut.texCoord0 = vIn.texCoord0;
	vOut.texCoord1 = vIn.texCoord1;
	vOut.texCoord2 = vIn.texCoord2;
	
	return vOut;
}

float4 PS_ScreenSpace(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

    return output;
}

technique Default
{
    pass p0
    {
        //SetVertexShader(CompileShader(vs_2_0, VS() ) );
        //SetGeometryShader(NULL);
		//SetPixelShader(CompileShader(ps_2_0, PS_LIGHT() ) );
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_LIGHT();
	}
}

technique ScreenSpace
{
    pass p0
    {
        //SetVertexShader(CompileShader(vs_2_0, VS_ScreenSpace()));
        //SetGeometryShader(NULL);
		//SetPixelShader(CompileShader(ps_2_0, PS_ScreenSpace()));
		
		VertexShader = compile vs_2_0 VS_ScreenSpace();
		PixelShader = compile ps_2_0 PS_ScreenSpace();
	}
}

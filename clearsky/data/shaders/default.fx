#include "light.inc"
#include "bumpmap.inc"

#define MATH_E 2.71828

#define MAX_TEXTURE_LAYERS 7

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
	
cbuffer cbPerFrame
{	
	float time;
	float3 gEyePosW;
	matrix view;
	matrix projection;
};

Material gMaterial;
cbuffer cbPerObject
{
	
	matrix world;
	matrix mvp;
};

//Texture2D gDiffuseMap;
Texture2D gAlphaMap;

//diffuse textures
Texture2D gTexture2D0;
Texture2D gTexture2D1;
Texture2D gTexture2D2;

Texture2D gSpecTexture2D;
Texture2D gHeightTexture2D;

int gNumTextureLayers = 1;

Light gLight[MAX_LIGHTS];

cbuffer cbRare
{
	Viewport gViewport;
	Fog gFog;
	bool gNormalsPosition = false;
	bool lightOn : register(b0)	 = false;
	bool fogOn	: register(b1)	 = false;
	bool textureOn  : register(b2) = false;
	bool alphaMapOn : register(b3) = false;
	bool specTextureOn  : register(b4) = false;
	bool heightTextureOn  : register(b5) = false;
};

float fTime = 1;
bool textureWrap = true;

SamplerState SamplerClamp
{
	Filter = ANISOTROPIC;
	AddressU = clamp;
    AddressV = clamp;
};

SamplerState DefaultSamplerState
{
	Filter = ANISOTROPIC;
	AddressU = wrap;
	AddressV = wrap;
};

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
	float3 tangent  : TANGENT;
	//float4 color    : COLOR;
	float2 texCoord0 : texCoord0;
	float2 texCoord1 : texCoord1;
	//float2 texCoord2 : texCoord2;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION; 
	float3 normalW : NORMAL;
	//float4 color    : COLOR;
	float3 tangentW : TANGENT;
    float3 posW    : POSITION;
	float2 texCoord0 : texCoord0;
	float2 texCoord1 : texCoord1;
	//float2 texCoord2 : texCoord2;
	float4 posCamera : CAMPOS;
};

//functions for fog
float fogValue(Fog fog, float distance)
{
	float value = 1.0;
	
	if(fog.fogType == FOG_TYPE_LINEAR)
	{
		value = (fog.end - distance)/(fog.end - fog.start);
	}
	else if(fog.fogType == FOG_TYPE_EXPONENT)
	{
		value = 1.0/pow(MATH_E, distance*fog.density);
	}
	else
	{
		value = 1.0/pow(MATH_E, distance*fog.density*distance*fog.density);
	}

	return clamp(value,0,1);
}

float4 setTextures(float4 outputColor, VS_OUT vertexInput)
{
	if(textureOn == true)
	{
		if(gNumTextureLayers>0)
		{
			outputColor = outputColor*gTexture2D0.Sample( DefaultSamplerState, vertexInput.texCoord0);

			if(gNumTextureLayers>1)
			{
				outputColor = outputColor*gTexture2D1.Sample( DefaultSamplerState, vertexInput.texCoord1);

				/*if(gNumTextureLayers>2)
				{
					outputColor = outputColor*gTexture2D2.Sample( DefaultSamplerState, vertexInput.texCoord2);

				}//end layer 2*/
			}//end layer 1
		}//end layer 0
	}//end texture on

	return outputColor;
}

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	//vIn.posL.xy += 0.5f*sin(vIn.posL.x)*sin(3*fTime);
	//vIn.posL.z *= 0.6f + 0.4f*sin(2*fTime);

	//matrix transform = mul(world, view);
	//transform = mul(mvp, projection);

	vOut.posH = mul(float4(vIn.posL,1.0f), mvp);

	float4 worldPos = mul(float4(vIn.posL, 1.0f), world);
	vOut.posCamera  = mul(worldPos, view);

	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), mvp);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), mvp);
	
	vOut.tangentW = mul(float4(vIn.tangent,0.0f),world);
			
	//vOut.color = vIn.color;
	
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
	float4 output = float4(0,0,0,1); //pIn.color;

	[branch]
	if(lightOn == true)
	{
		// Interpolating normal can make it not be of unit length so normalize it.
		pIn.normalW = normalize(pIn.normalW);

		float3 litColor = float3(0,0,0);
		float4 diffuseColor = gMaterial.diffuse;
		float4 ambientColor = gMaterial.ambient;
		float4 specularColor = gMaterial.specular;

		if(textureOn==true)
		{
			diffuseColor = gTexture2D0.Sample( DefaultSamplerState, pIn.texCoord0);//setTextures(output, pIn);
		}

		if(specTextureOn == true)
			specularColor = gSpecTexture2D.Sample(DefaultSamplerState, pIn.texCoord0);

		specularColor.a *= 256.0f;

		if(heightTextureOn==true)
		{
			/*float offset = 0.01;

			float3 x1 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(-offset,0));
			float3 x2 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(offset,0));

			float3 y1 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(0,-offset));
			float3 y2 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(0,offset));

			float3 s = x2-x1;
			float3 t = y2-y1;
			//float3 y1 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+1);
			//float3 y2 =  gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0-1);

			float p = gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0).x;
			float x = gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(offset,0)).x;
			float y = gHeightTexture2D.Sample(SamplerClamp, pIn.texCoord0+float2(0,offset)).x;

			//pIn.normalW = cross(s,t);
			pIn.normalW.x = s;
			pIn.normalW.y = t;

			pIn.normalW = normalize(pIn.normalW);*/

			float3 normalT = gHeightTexture2D.Sample(DefaultSamplerState, pIn.texCoord0);
			normalT = 2.0f*normalT-1.0f;

			float3 N = normalize(pIn.normalW);
			float3 T = normalize(pIn.tangentW-dot(pIn.tangentW,N)*N);
			float3 B = cross(N,T);

			float3x3 TNB = float3x3(T,B,N);

			pIn.normalW = bumpmapping(gHeightTexture2D, DefaultSamplerState, pIn.texCoord0, pIn.normalW, pIn.tangentW);
			
			//normalize(mul(normalT,TNB));
		}

	    //TODO: maybe using maximum lights, setting disabled ones to back ist faster solution
		for(uint i=0; i<MAX_LIGHTS; i++)
		{
			//[branch]
			if(gLight[i].active)
			{
				[branch]
				if(gLight[i].lightType == LIGHT_TYPE_PARALLEL ) // Parallel
				{
					
					litColor += ParallelLight(gLight[i], diffuseColor, ambientColor, specularColor, gEyePosW, pIn.posW, pIn.normalW);
					
				}
				else if(gLight[i].lightType == LIGHT_TYPE_POINT ) // Point
				{
					litColor += PointLight(gLight[i], gMaterial, gEyePosW, pIn.posW, pIn.normalW);
				}
				else // Spot
				{
					litColor += Spotlight(gLight[i], gMaterial, gEyePosW, pIn.posW, pIn.normalW);
				}

			}
		}

		output = float4(litColor, diffuseColor.a);

		/*if(heightTextureOn==true)
		{
			output = gHeightTexture2D.Sample(DefaultSamplerState, pIn.texCoord0);//setTextures(output, pIn);
		}*/

	}//end if
	else
	{
		if(textureOn==true)
		{
			output = gTexture2D0.Sample( DefaultSamplerState, pIn.texCoord0);//setTextures(output, pIn);
		}
	}	
	
	//output = output*setTextures(output, pIn);

		//output = output*gSpecTexture2D.Sample(DefaultSamplerState, pIn.texCoord0);

	[branch]
	if(alphaMapOn == true)
		output.a = output.a*gAlphaMap.Sample( SamplerClamp, pIn.texCoord0);

	if(fogOn == true)
	{
		float fogC = fogValue(gFog, pIn.posCamera.z);

		output = fogC*output+(1.0 - fogC)*gFog.color;
	}

    return output;//output*pIn.color;
}

//screenspace vertex format
struct VS_SP_IN
{
	float3 posL     : POSITION;
	float4 color    : COLOR;
	float2 texCoord0 : texCoord;
};

struct VS_SP_OUT
{
	float4 posH    : SV_POSITION; 
	float4 color    : COLOR;
	float3 posW    : POSITION;
	float2 texCoord0 : texCoord;
};

VS_SP_OUT VS_ScreenSpace(VS_SP_IN vIn)
{
	VS_SP_OUT vOut;
	
	vOut.posH.x = (vIn.posL.x / (gViewport.width/2.0))-1;
	vOut.posH.y = -(vIn.posL.y / (gViewport.height/2.0))+1;
	vOut.posH.z = vIn.posL.z;
	vOut.posH.w = 1;

	vOut.posW    = float4(vIn.posL, 1.0f);
			
	vOut.color = vIn.color;
		
	vOut.texCoord0 = vIn.texCoord0;
	
	return vOut;
}

float4 PS_ScreenSpace(VS_SP_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

	//float4 output = float4(pIn.color.r, pIn.color.g, pIn.color.b, gMaterial.diffuse.a);

	if(textureOn==true)
		output = output*gTexture2D0.Sample(DefaultSamplerState, pIn.texCoord0);

	if(alphaMapOn == true)
		output.a = output.a*gAlphaMap.Sample( SamplerClamp, pIn.texCoord0);

    return output;
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

technique10 ScreenSpace
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_ScreenSpace()) );
        SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_ScreenSpace()) );
	}
}

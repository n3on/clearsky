#define MATH_E 2.71828

#define MAX_LIGHTS 6

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

Light gLight[MAX_LIGHTS];

cbuffer cbRare
{
	Viewport gViewport;
	Fog gFog;
	bool gNormalsPosition = false;
	bool lightOn : register(b0)	 = false;
	bool fogOn	: register(b1)	 = true;
	bool textureOn  : register(b2) = false;
	bool alphaMapOn : register(b3) = false;
};

bool textureWrap = true;

SamplerState SamplerClamp
{
	Filter = ANISOTROPIC;
	AddressU = clamp;
    AddressV = clamp;
};

SamplerState SamplerWrap
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
	float4 color    : COLOR;
	float2 texCoord0 =  : TEX;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION; 
	float3 normalW : NORMAL;
	float4 color    : COLOR;
    float3 posW    : POSITION;
	float2 texCoord0 =  : TEX;
	float4 posCamera : CAMPOS;
};


float3 ParallelLight(Light L, Material material, float3 eyePos, float3 pos, float3 normal)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir;
	
	// Add the ambient term.
	litColor = material.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = max(dot(lightVec, normal),0);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.specular.a, 1.0f);
		float3 toEye     = normalize(eyePos - pos);
		float3 R         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse * L.diffuse;
		litColor += specFactor * material.specular * L.spec;
	}
	
	return litColor;
}

float3 PointLight(Light L, Material material, float3 eyePos, float3 pos, float3 normal)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = L.pos - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > L.range )
		return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor = material.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = dot(lightVec, normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.specular.a,1.0f);
		float3 toEye     = normalize(eyePos - pos);
		float3 R         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
	
		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse * L.diffuse;
		litColor += specFactor * material.specular * L.spec;
	}
	
	// attenuate
	//float3 att = float3(L.att0, L.att1, L.att2);
	float att = 1/(L.att0+(L.att1*d)+(L.att2*d*d));
	return litColor *att; /// dot(att, float3(1.0f, d, d*d));
}

float3 Spotlight(Light L, Material material, float3 eyePos, float3 pos, float3 normal)
{
	float3 litColor = PointLight(L, material, eyePos, pos, normal);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(L.pos - pos);
	
	float s = pow(max(dot(-lightVec, L.dir), 0.0f), L.spotPower);
	
	// Scale color by spotlight factor.
	return litColor*s;
}


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

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	matrix transform = mul(world, view);
	transform = mul(transform, projection);

	vOut.posH = mul(float4(vIn.posL,1.0f), transform);

	float4 worldPos = mul(float4(vIn.posL, 1.0f), world);
	vOut.posCamera  = mul(worldPos, view);

	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), transform);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), transform);
			
	vOut.color = vIn.color;
		
	// Output vertex attributes for interpolation across triangle.
	//vOut.diffuse = vIn.diffuse;
	//vOut.spec    = vIn.spec;
	
	vOut.texCoord0 = 0 = vIn.texCoord0 = ;
	
	return vOut;
}
 
float4 PS_LIGHT(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

	[branch]
	if(lightOn == true)
	{
		// Interpolating normal can make it not be of unit length so normalize it.
		pIn.normalW = normalize(pIn.normalW);
	   
		SurfaceInfo v = {pIn.posW, pIn.normalW, gMaterial.diffuse/*pIn.color*/, gMaterial.specular/*pIn.spec*/};
	    
		float3 litColor = float3(0,0,0);
	  
		for(uint i=0; i<MAX_LIGHTS; i++)
		{
			//[branch]
			if(gLight[i].active)
			{
				[branch]
				if(gLight[i].lightType == LIGHT_TYPE_PARALLEL ) // Parallel
				{
					
					litColor += ParallelLight(gLight[i], gMaterial, gEyePosW, pIn.posW, pIn.normalW);
					
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
		
		output = output*float4(litColor, gMaterial.diffuse.a);
	}//end else		
	
	[branch]
	if(textureOn == true)
		output = output*gDiffuseMap.Sample( SamplerClamp, pIn.texCoord0 = );

	[branch]
	if(alphaMapOn == true)
		output.a = output.a*gAlphaMap.Sample( SamplerClamp, pIn.texCoord0 = );

	if(fogOn == true)
	{
		float fogC = fogValue(gFog, pIn.posCamera.z);

		output = fogC*output+(1.0 - fogC)*gFog.color;
	}

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

	vOut.posW    = float4(vIn.posL, 1.0f);
	vOut.normalW = float4(vIn.normalL, 0.0f);
			
	vOut.color = vIn.color;
		
	//Output vertex attributes for interpolation across triangle.
	//vOut.diffuse = vIn.diffuse;
	//vOut.spec    = vIn.spec;
	
	vOut.texCoord0 = 0 = vIn.texCoord0 = ;
	
	return vOut;
}

float4 PS_ScreenSpace(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

	//float4 output = float4(pIn.color.r, pIn.color.g, pIn.color.b, gMaterial.diffuse.a);

	if(textureOn == true)
		output   = output*gDiffuseMap.Sample( SamplerClamp, pIn.texCoord0 = );

	if(alphaMapOn == true)
			output.a = output.a*gAlphaMap.Sample( SamplerClamp, pIn.texCoord0 = );

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

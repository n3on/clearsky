
#define MAX_LIGHTS 3

#define LIGHT_TYPE_PARALLEL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#define FOG_TYPE_LINEAR 0
#define FOG_TYPE_EXPONENT 1
#define FOG_TYPE_EXPONENT2 2

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

struct Material
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float4 emissive;
};

float3 ParallelLight(Light L, float4 diffuse, float4 ambient, float4 specularColor, float3 eyePos, float3 pos, float3 normal)
{
	float3 litColor = float3(0,0,0);

	L.dir = normalize(L.dir);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir;
	
	// Add the ambient term.
	litColor = diffuse.rgb * L.diffuse.rgb;	
	litColor += ambient * L.ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = dot(lightVec, normal);
	//[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(specularColor.a, 1.0f);
		float3 toEye     = normalize(eyePos - pos);
		float3 R         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		// diffuse and specular terms
		litColor += diffuseFactor * diffuse.rgb * L.diffuse.rgb;
		litColor += specFactor * specularColor.rgb * L.spec.rgb;
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
	litColor = material.ambient * L.ambient;	
	
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
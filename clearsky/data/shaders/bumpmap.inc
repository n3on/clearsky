
float3 bumpmapping(Texture2D normalMap, SamplerState normalSamplerState, float2 texCoords, float3 normalW, float3 tangentW)
{
	float3 normalT = normalMap.Sample(normalSamplerState, texCoords);
	normalT = 2.0f*normalT-1.0f;

	float3 N = normalize(normalW);
	float3 T = normalize(tangentW-dot(tangentW,N)*N);
	float3 B = cross(N,T);

	float3x3 TNB = float3x3(T,B,N);

	return normalize(mul(normalT,TNB));
}
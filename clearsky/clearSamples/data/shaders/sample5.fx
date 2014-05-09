cbuffer cbPerFrame
{	
	matrix view;
	matrix projection;
};


cbuffer cbPerObject
{
	matrix world;
};

struct VS_IN
{
	float3 posL     : POSITION;
	float4 color    : COLOR;
	
};

struct VS_OUT
{
	float4 posH    : SV_POSITION; 
	float4 color    : COLOR;

};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	matrix transform = mul(world, view);
	transform = mul(transform, projection);

	vOut.posH = mul(float4(vIn.posL,1.0f), transform);
	
	vOut.color = vIn.color;
		

	return vOut;
}
 
float4 PS(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

    return output;
}

technique10 Default
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}

}

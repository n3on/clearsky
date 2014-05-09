cbuffer cbPerFrame
{	
	int Time;
	matrix view;
	matrix projection;
};


cbuffer cbPerObject
{
	matrix world;
};

RasterizerState WireframeState
{
	FillMode = Wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState SolidState
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
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

	vIn.posL.x += 0.3f*sin(vIn.posL.y*Time*0.001)+0.1*sin(vIn.posL.z*Time*0.001);
	vIn.posL.y += sin(vIn.posL.y*Time*0.001)*cos(vIn.posL.y*Time*0.001);
	vIn.posL.z += 0.5f*sin(vIn.posL.z*Time*0.001);
	//vIn.posL.xy += 0.5f*sin(vIn.posL.x)*sin(3*Time*0.001);
	//vIn.posL.z  *= 0.6f+0.4f*sin(2*Time*0.001);
	
	vOut.posH = mul(float4(vIn.posL,1.0f), transform);
	
	vOut.color = vIn.color;
		

	return vOut;
}

VS_OUT VS_WIRE(VS_IN vIn)
{
	VS_OUT vOut;
	
	matrix transform = mul(world, view);
	transform = mul(transform, projection);

	vIn.posL.x = 5;
	vIn.posL.y += 5;
	vIn.posL.z += 5;
	//vIn.posL.xy += 0.5f*sin(vIn.posL.x)*sin(3*Time*0.001);
	//vIn.posL.z  *= 0.6f+0.4f*sin(2*Time*0.001);
	
	vOut.posH = mul(float4(vIn.posL,1.0f), transform);
	
	vOut.color = vIn.color;
		

	return vOut;
}
 
float4 PS(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

	output.xyz -= sin(Time*0.001)*cos(Time*0.001);
	
    return output;
}

float4 PS_WIRE(VS_OUT pIn) : SV_Target
{
	float4 output = pIn.color;

	output.rgb = 0.0f;
	//pIn.color.r = 1.0f;
	//output.xyz -= sin(Time*0.001)*cos(Time*0.001);
	
    return output;
}

technique10 Default
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetRasterizerState(SolidState);
	}
	
	pass P1
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_WIRE() ) );
		
		SetRasterizerState(WireframeState);
	}
}

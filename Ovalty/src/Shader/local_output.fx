#include "light_local.fx"

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	float4x4 gView;
	float4x4 gProj;
	float gDim;
	float3 gVoxelOffset;
	float gVoxelSize;
	float gTime;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
};

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
	float2 tex   : TEXCOORD;
	uint index	: SV_VertexID;
};

struct VS_OUT
{
	float4 posW   : POSITION;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
	float2 tex    : TEXCOORD;
	uint ID		  :ID;
};

VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;
	vout.posW = mul(float4(vin.posL, 1.0f), gWorld);

	vout.posH = mul(vout.posW, gView);
	vout.posH = mul(vout.posH, gProj);

	vout.normW = mul(float4(vin.normL, 1.0f), gWorldInverTrans).xyz;
	vout.tex = vin.tex;

	vout.ID = vin.index;

	return vout;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	//set material
	MaterialLocal mat;
	setMatCornellBox(pin.ID,mat);

	//set point light
	PointLightLocal light;
	setPointLightLocal(light, gTime);

	float3 N = normalize(pin.normW);

	float3 toEyeW = normalize(gEyePosW - pin.posW.xyz);

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputePointLight(mat, light, pin.posW.xyz, N, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	float3 litColor = ambient.xyz + diffuse.xyz + spec.xyz;

	return float4(litColor, 1.0f);
}

RasterizerState SolidRS
{
	FillMode = SOLID;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

technique11 ConeTracingTech
{
	pass ConeTracingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetRasterizerState(SolidRS);
	}
}
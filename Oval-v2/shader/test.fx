#include "common_tools.fx"
//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame
{
	Matrix cb_Matrix;
	Voxel  cb_Voxel;
	Common cb_Common;
};
 
//------------------------
//VERTEX SHADER
//------------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
	float2 tex   : TEXCOORD;
};

struct VS_OUT
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;

	vout.posH= mul(float4(vin.posL, 1.0f), cb_Matrix.world);
	vout.posH = mul(vout.posH, cb_Matrix.view); 
	vout.posH = mul(vout.posH, cb_Matrix.proj);

	vout.normW = mul(float4(vin.normL, 1.0f), cb_Matrix.world).xyz;

	vout.tex = vin.tex;

	return vout;
}

//------------------------
//PIXEL SHADER
//------------------------
struct PS_IN
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

float4 ps_main(PS_IN pin) : SV_TARGET
{

	return float4(pin.normW, 1.0f);
}

technique11 TEST_TECH
{
	pass TEST_PASS
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps_main()));

	}
}
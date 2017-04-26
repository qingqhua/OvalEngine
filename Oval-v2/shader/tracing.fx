#include "common_tools.fx"
#include "BRDF_tools.fx"

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame
{
	Matrix cb_Matrix;
	Voxel  cb_Voxel;
	Common cb_Common;
};

//--------------------------
//read/write 3d texture
//---------------------------
Texture3D<float4> uav_color;

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
	float4 posW   : POSITION;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
	float2 tex    : TEXCOORD;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;

	vout.posW = mul(float4(vin.posL, 1.0f), cb_Matrix.world);

	vout.posH = mul(vout.posW, cb_Matrix.view);
	vout.posH = mul(vout.posH, cb_Matrix.proj);

	
	vout.normW = mul(float4(vin.normL, 1.0f), cb_Matrix.world);

	vout.tex = vin.tex;

	return vout;
}

//------------------------
//PIXEL SHADER
//------------------------
struct PS_IN
{
	float4 posW   : POSITION;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
	float2 tex    : TEXCOORD;
};

float4 ps_main(PS_IN pin) : SV_TARGET
{
	float3 N = normalize(pin.normW);
	float3 V = normalize(cb_Common.eyeposW - pin.posW.xyz);
	float4 directlighting = 0.0f;

	MaterialBRDF mat;
	setMatCornellBox(0, mat);

	PointLightBRDF light[LIGHT_NUM];
	setPointLight(light[0], light[1], cb_Common.time);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec = light[i].position - pin.posW.xyz;
		float3 L = normalize(lightVec);
		float3 H = normalize(V + L);

		directlighting += DirectLighting(N, H, lightVec, V, L, light[i], mat);
	}
	
	//return directlighting / LIGHT_NUM;
	return uav_color[uint3(0, 0, 0)];
}

technique11 TRACING_TECH
{
	pass TRACING_PASS
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
}
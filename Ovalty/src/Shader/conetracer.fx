#include "tools.fx"

#define MAX_DIST 10.0f

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	PointLightBRDF gPointLight;
	float4x4 gView;
	float4x4 gProj;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
	MaterialBRDF gMat;
};

//--------------------------
//read voxel from 3d texture
//---------------------------
Texture3D<float4> gVoxelList;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
	float2 tex   : TEXCOORD;
};

struct VS_OUT
{
	float4 posW   : POSITION;
	uint3 pos_svo : SVO;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
	float2 tex    : TEXCOORD;
};

//--------------------------
//cone tracing
//---------------------------
float4 conetracing(float3 dir,float tanHalfAngle,float3 svo)
{
	float3 color=0.0f;
	float alpha=0.0f;

	float dist=1.0f;
	float3 startPos=svo;

	while(dist<MAX_DIST && alpha<1.0f)
	{
		float diameter=2.0f*tanHalfAngle*dist;
		float lodLevel=log2(diameter);
		float f=1.0f-alpha;

		float3 ray=startPos+dir*dist;

		float4 voxelColor=gVoxelList.SampleLevel(SVOFilter, ray/256.0f ,lodLevel);
		color += f * voxelColor.rgb;
		alpha += f*voxelColor.a;

        dist += diameter * 0.5; 
	}

	//todo it seems that pos_svo and world_to_svo(posW) have different result, dont know thy
	//float4 voxelColor=gVoxelList.SampleLevel(gAnisotropicSam, world_to_svo(ray,256.0f)/256.0f ,0);
	
	return float4(color,alpha);
}

//--------------------------
//composite indirect lighting
//---------------------------
float4 InDirectLighting(float3 N,float3 svo)
{
	float4 color=0.0f;
	
	float4 occlusion=0.0f;

	color+=conetracing(N,1,svo);
	color+=conetracing(N*cos(PI/3),0.577,svo);
	color+=conetracing(N*cos(PI/6),0.577,svo);

	return color;
}

//-----------------------------
//VERTEX SHADER
//-----------------------------
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;
	vout.posW=mul(float4(vin.posL,1.0f),gWorld);
	vout.pos_svo=ceil(vout.posW+128.0f);
	
	vout.posH=mul(vout.posW,gView);
	vout.posH=mul(vout.posH,gProj);

	vout.normW=mul(float4(vin.normL,1.0f),gWorldInverTrans).xyz;
	vout.tex=vin.tex;
	return vout;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	float3 lightVec=gPointLight.position-pin.posW;

	float3 N=normalize(pin.normW);
	float3 V=normalize(gEyePosW-pin.posW);
	float3 L= normalize(lightVec);
	float3 H=normalize(V+L);

	float4 directlighting = DirectLighting(N, H, lightVec, V, L,gPointLight,gMat);
	float4 indirectlighting=InDirectLighting(N,pin.pos_svo);

	return indirectlighting;
}

RasterizerState SolidRS
{
	FillMode = SOLID;
	CullMode = None;
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
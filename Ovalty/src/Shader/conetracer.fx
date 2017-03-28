#include "tools.fx"

#define MAX_DIST 10.0f
//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	PointLightBRDF gPointLight[2];
	float4x4 gView;
	float4x4 gProj;
	float gDim;
	float3 gVoxelOffset;
	float gVoxelSize;
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

//--------------------------
//cone tracing
//---------------------------
float4 conetracing(float3 dir,float theta,float3 posW)
{
	float3 color=0.0f;
	float alpha=0.0f;

	float dist=1.0f;
	float3 startPos=posW;
	float tanHalfAngle=tan(theta);

	while(dist<MAX_DIST )
	{
		float diameter=2.0f*tanHalfAngle*dist;
		float lodLevel=log2(diameter);
		//float f=1.0f-alpha;

		float3 ray=startPos+dir*dist;

		float3 ray_svo= world_to_svo(ray,gDim,gVoxelSize,gVoxelOffset);

		float4 voxelColor=gVoxelList.SampleLevel(SVOFilter, ray_svo/256.0f ,lodLevel);
		color += voxelColor.rgb;
		//alpha += f*voxelColor.a;

        dist += diameter * 0.5; 
	}
	
	return float4(color,alpha);
}

//--------------------------
//composite indirect lighting
//---------------------------
float4 InDirectLighting(float3 N,float3 posW)
{
	float4 color=0.0f;
	
	float4 occlusion=0.0f;

	color+=conetracing(N,PI/2,posW);
	color+=conetracing(N*cos(PI/3),PI/3,posW);
	color+=conetracing(N*cos(PI/6),PI/3,posW);
	color+=conetracing(N*cos(PI),PI/3,posW);
	color+=conetracing(N*cos(PI/2),PI/3,posW);
	color+=conetracing(N*cos(PI/2*3),PI/2,posW);
	color+=conetracing(N*cos(PI/6*3),PI/3,posW);

	return color;
}

//-----------------------------
//VERTEX SHADER
//-----------------------------
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;
	vout.posW=mul(float4(vin.posL,1.0f),gWorld);
	 
	vout.posH=mul(vout.posW,gView);
	vout.posH=mul(vout.posH,gProj);

	vout.normW=mul(float4(vin.normL,1.0f),gWorldInverTrans).xyz;
	vout.tex=vin.tex;

	vout.ID=vin.index;

	return vout;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	float3 N=normalize(pin.normW);
	float3 V=normalize(gEyePosW-pin.posW);
	float4 directlighting=0.0f;

	MaterialBRDF mat;
	setMatPerObject(pin.ID,mat);

	for(int i=0;i<1;i++)
	{
		float3 lightVec=gPointLight[i].position-pin.posW;
		float3 L= normalize(lightVec);
		float3 H=normalize(V+L);

		directlighting += DirectLighting(N, H, lightVec, V, L,gPointLight[i],mat);
	}

	float4 indirectlighting=InDirectLighting(N,pin.posW);
	
	return indirectlighting;
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
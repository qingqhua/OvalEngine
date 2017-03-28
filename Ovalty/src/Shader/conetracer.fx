#include "tools.fx"

#define MAX_DIST 10.0f
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
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
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
float4 DiffuseCone(float3 dir,float theta,float3 posW)
{
	float3 color=0.0f;
	float alpha=0.0f;

	float dist=0.1f;
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
		color += 0.1*voxelColor.rgb;
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
	
	const float w[3]={1.0,1.0,1.0};

	const float3 orth=normalize(orthogonal(N));
	const float3 orth2=normalize(cross(orth,N));

	const float3 corner=normalize(orth+orth2);
	const float3 corner2=normalize(orth-orth2);

	color+=w[0]*DiffuseCone(N,PI/3,posW);

	color+=w[1]*DiffuseCone(lerp(N,orth,0.5f),PI/3,posW);
	color+=w[1]*DiffuseCone(lerp(N,-orth,0.5f),PI/3,posW);
	color+=w[1]*DiffuseCone(lerp(N,orth2,0.5f),PI/3,posW);
	color+=w[1]*DiffuseCone(lerp(N,-orth2,0.5f),PI/3,posW);

	color+=w[2]*DiffuseCone(lerp(N,corner,0.5f),PI/3,posW);
	color+=w[2]*DiffuseCone(lerp(N,-corner,0.5f),PI/3,posW);
	color+=w[2]*DiffuseCone(lerp(N,corner2,0.5f),PI/3,posW);
	color+=w[2]*DiffuseCone(lerp(N,-corner2,0.5f),PI/3,posW);

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

	PointLightBRDF light[LIGHT_NUM];
	setPointLight(light[0],light[1]);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec=light[i].position-pin.posW;
		float3 L= normalize(lightVec);
		float3 H=normalize(V+L);

		directlighting += DirectLighting(N, H, lightVec, V, L,light[i],mat);
	}

	float3 ray_svo= world_to_svo(pin.posW,gDim,gVoxelSize,gVoxelOffset);
	float4 indirectlighting=InDirectLighting(N,pin.posW);

	float3 ray_sv= world_to_svo(pin.posW,gDim,gVoxelSize,gVoxelOffset);
		float4 voxelColor=gVoxelList.SampleLevel(SVOFilter, ray_sv/256.0f ,0);
	return voxelColor;
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
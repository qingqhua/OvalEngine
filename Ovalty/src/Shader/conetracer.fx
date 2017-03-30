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
	float3 pos_svo :SVO;
};

//--------------------------
//cone tracing
//---------------------------
float4 ConeTracing(float3 dir,float theta,float3 posW,float3 N)
{
	float3 color=0.0f;
	float alpha=1.0f;

	
	float3 startPos=posW+N*gVoxelSize;
	float samplestep=gVoxelSize/2;
	float tanHalfAngle=tan(theta/2);
	float tanHalfAngle2=tan(theta/2/4);
	float sampleFactor=(1+tanHalfAngle2)/(1-tanHalfAngle2);
	float dist=0;
	while(dist<1.0f)
	{
		samplestep*=sampleFactor;
		dist+=gVoxelSize/2;
		float diameter=2.0f*tanHalfAngle*dist;
		float lodLevel=log2(diameter);

		float3 ray = startPos+dir*dist;
		float3 ray_svo = world_to_svo(ray,gVoxelSize,gVoxelOffset);

		if(!(all(ray_svo>= 0) && all(ray_svo <= gDim))) 
			break;		

		color+=0.3*gVoxelList.SampleLevel(SVOFilter,ray_svo/gDim+0.5f/gDim,lodLevel).xyz;
	}
	
	return float4(color,alpha);
}

float4 SpecularCone(float3 V,float3 N,float3 posW)
{
	float4 color=0.0f;

	float3 v=1.0f*V;
	float3 r=reflect(v,N);
	float angle=PI/2;

	color=ConeTracing(r,angle,posW,N);
	return color;
}

//--------------------------
//composite indirect lighting
//---------------------------
float4 DiffuseCone(float3 N,float3 posW)
{
	float4 color=0.0f;
	
	const float w[3]={1.0,1.0,1.0};
	int conenum=9;

	const float3 orth=normalize(orthogonal(N));
	const float3 orth2=normalize(cross(orth,N));

	const float3 corner=normalize(orth+orth2);
	const float3 corner2=normalize(orth-orth2);

	color+=w[0]*ConeTracing(N,PI/3,posW,N);

	color+=w[1]*ConeTracing(lerp(N,orth,0.5f),PI/3,posW,N);
	color+=w[1]*ConeTracing(lerp(N,-orth,0.5f),PI/3,posW,N);
	color+=w[1]*ConeTracing(lerp(N,orth2,0.5f),PI/3,posW,N);
	color+=w[1]*ConeTracing(lerp(N,-orth2,0.5f),PI/3,posW,N);

	color+=w[2]*ConeTracing(lerp(N,corner,0.5f),PI/3,posW,N);
	color+=w[2]*ConeTracing(lerp(N,-corner,0.5f),PI/3,posW,N);
	color+=w[2]*ConeTracing(lerp(N,corner2,0.5f),PI/3,posW,N);
	color+=w[2]*ConeTracing(lerp(N,-corner2,0.5f),PI/3,posW,N);

	return color/conenum;
}

float4 IndirectLighting(float3 N,float3 V,float3 posW,MaterialBRDF mat )
{
	float4 diff=DiffuseCone(N,posW)*float4(mat.albedo*(1.0-mat.metallic),1.0f);
	//float4 diff=DiffuseCone(N,posW);
	float4 spec= SpecularCone(V,N,posW);
	return diff;
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

	vout.pos_svo=world_to_svo(vout.posW,gVoxelSize,gVoxelOffset);

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
	setMatEmptyCornell(pin.ID,mat);

	PointLightBRDF light[LIGHT_NUM];
	setPointLight(light[0],light[1]);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec=light[i].position-pin.posW;
		float3 L= normalize(lightVec);
		float3 H=normalize(V+L);

		directlighting += DirectLighting(N, H, lightVec, V, L,light[i],mat);
	}

	float4 indirectlighting=IndirectLighting(N,V,pin.posW,mat);

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
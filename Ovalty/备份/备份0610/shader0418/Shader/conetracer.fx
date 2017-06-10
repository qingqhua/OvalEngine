#include "common_tools.fx"
#include "brdf_tools.fx"

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
float4 ConeTracing(float3 dir,float3 startPos)
{
	dir = normalize(dir);
	
	float cone_ratio = 1.0f;
	float4 accum = 0.0f;

	float min_voxel_diameter = gVoxelSize;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float dist = min_voxel_diameter;

	while(dist<1.5)
	{
		float diameter = max(min_voxel_diameter, cone_ratio * dist);
		float lodLevel = log2(dist /gVoxelSize)-1;

		float3 ray = startPos + dir*dist;
		ray = world_to_svo(ray, gVoxelSize, gVoxelOffset);

		if (!(all(ray >= 0) && all(ray <= gDim)))
			break;

		float4 color = gVoxelList.SampleLevel(SVOFilter, ray / gDim +0.5/gDim, lodLevel);

		float a = 1.0f - accum.a;

		accum += color*a;
		dist += gVoxelSize;
	}
	
	return accum;
}

//--------------------------
//composite indirect lighting
//---------------------------
float4 DiffuseCone(float3 N,float3 posW)
{
	float4 color=0.0f;
	float3 pos = posW ;
	
	const float3 orth = normalize(orthogonal(N));
	const float3 orth2 = normalize(cross(orth, N));

	const float3 corner = normalize(orth + orth2);
	const float3 corner2 = normalize(orth - orth2);
	
#define cone_num 9

	color+=ConeTracing(N,posW);

	color+=ConeTracing(lerp(N,orth,0.5f),pos);
	color+=ConeTracing(lerp(N,-orth,0.5f),pos);
	color+=ConeTracing(lerp(N,orth2,0.5f),pos);
	color+=ConeTracing(lerp(N,-orth2,0.5f),pos);

	color+=ConeTracing(lerp(N,corner,0.5f),pos);
	color+=ConeTracing(lerp(N,-corner,0.5f),pos);
	color+=ConeTracing(lerp(N,corner2,0.5f),pos);
	color+=ConeTracing(lerp(N,-corner2,0.5f),pos);

	//color *= 4.0 * PI / cone_num;
	//return color / PI;
	return color / cone_num;
}

float4 IndirectLighting(float3 N,float3 V,float3 posW,MaterialBRDF mat )
{
	float4 diff=DiffuseCone(N,posW);
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

	return vout;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	float3 N = normalize(pin.normW);
	float3 V = normalize(gEyePosW - pin.posW.xyz);
	float4 directlighting = 0.0f;
	float4 shadow = 0.0f;
	float4 indirectlighting = 0.0f;

	MaterialBRDF mat;
	setMatCornellBox(pin.ID, mat);

	PointLightBRDF light[LIGHT_NUM];
	setPointLight(light[0], light[1]);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec = light[i].position - pin.posW.xyz;
		float3 L = normalize(lightVec);
		float3 H = normalize(V + L);

		directlighting += DirectLighting(N, H, lightVec, V, L, light[i], mat);
	}

	indirectlighting += IndirectLighting(N, V, pin.posW, mat);
	
	
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
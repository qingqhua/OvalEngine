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
//cone_ratio: diameter / max_dist.
//---------------------------
float4 ConeTracing(float3 dir,float3 startPos,float cone_ratio)
{
	float4 accum = 0.0f;

	dir = normalize(dir);

	float dist = 0;

	while(dist<2)
	{
		float diameter = cone_ratio*dist;
		float lodLevel = log2(diameter / gVoxelSize);

		float3 ray = startPos + dir*dist;
		float3 ray_svo = world_to_svo(ray, gVoxelSize, gVoxelOffset);

		if (!(all(ray_svo > 0) && all(ray_svo < gDim)))
			break;

		float4 color = gVoxelList.SampleLevel(SVOFilter, ray_svo / gDim + 0.5f / gDim, lodLevel);

		float a = 1.0f - accum.a;
		accum += color*a;

		dist += gVoxelSize;
	}
	
	return accum;
}

float4 ShadowConeTracing(float3 dir,float3 startPos,float dist_L)
{
	float acc = 0;

	float dist = gVoxelSize/8;
	const float STOP = dist_L - 16 * gVoxelSize;

	while(dist < STOP ){	
		float3 ray = startPos + dist * dir;
		float3 ray_svo = world_to_svo(ray,gVoxelSize,gVoxelOffset);

		if(!(all(ray_svo> 0) && all(ray_svo < gDim))) 
			break;		

		float l = pow(dist, 2); 
		float s1 = 0.062 * gVoxelList.SampleLevel(SVOFilter,ray_svo/gDim+0.5f/gDim, 1 + 0.75 * l).a;
		float s2 = 0.135 * gVoxelList.SampleLevel(SVOFilter,ray_svo/gDim+0.5f/gDim, 4.5 * l).a;
		float s = s1 + s2;
		acc += (1 - acc) * s;
		dist += 0.9 * gVoxelSize * (1 + 0.05 * l);
	}
	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}

float4 SpecularCone(float3 V,float3 N,float3 posW)
{
	float4 color=0.0f;

	float3 dir = reflect(V, N);

	float cone_ratio =2.0f;
	float3 offset = N*gVoxelSize*1.414*2;
	color = ConeTracing(dir, posW + offset, cone_ratio);

	return color;
}

//--------------------------
//composite indirect lighting
//---------------------------
float4 DiffuseCone(float3 N,float3 posW)
{
	//accum color
	float4 color=0.0f;
	
	//directions
	const float3 orth=normalize(orthogonal(N));
	const float3 orth2=normalize(cross(orth,N));

	const float3 corner=normalize(orth+orth2);
	const float3 corner2=normalize(orth-orth2);

#define cone_num 9

	float3 dir[cone_num] = {
		//normal direction
		N,

		//4 side directions
		lerp(N,orth,0.5f),
		lerp(N,-orth,0.5f),
		lerp(N,orth2,0.5f),
		lerp(N,-orth2,0.5f),

		//4 corner directions
		lerp(N,corner,0.5f),
		lerp(N,-corner,0.5f),
		lerp(N,corner2,0.5f),
		lerp(N,-corner2,0.5f)
	};

	//60 degree cone for diffuse cone
	float cone_ratio = 2.0f / 1.732f;

	// trace 9 diffuse cones
	for (uint i = 0; i < cone_num; i++)
	{
		//TODO why change offse t makes result wrong
		float3 offset = N *gVoxelSize*1.414 * 2;
		color += ConeTracing(dir[i], posW + offset, cone_ratio);
	}
	
	//average
	color *= 4.0 * PI / cone_num;
	return color / PI;
	//return color/ cone_num;
}

float4 IndirectLighting(float3 N,float3 V,float3 posW,MaterialBRDF mat )
{
	float4 diff = DiffuseCone(N, posW)*float4(mat.albedo,1.0f);

	//TODO modify specular part
	float4 spec= SpecularCone(V,N,posW)*float4(mat.albedo, 1.0f);
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
	float3 N=normalize(pin.normW);
	float3 V=normalize(gEyePosW-pin.posW.xyz);
	float4 directlighting=0.0f;
	float4 shadow=0.0f;

	MaterialBRDF mat;
	setMatCornellBox(pin.ID,mat);

	PointLightBRDF light[LIGHT_NUM];
	setPointLight(light[0],light[1]);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec=light[i].position-pin.posW.xyz;
		float3 L= normalize(lightVec);
		float3 H=normalize(V+L);

		directlighting += DirectLighting(N, H, lightVec, V, L,light[i],mat);
		shadow+=ShadowConeTracing(N,pin.posW,length(lightVec));
	}

	float4 indirectlighting=IndirectLighting(N,V,pin.posW,mat);
	
	return 0.4*(directlighting+ indirectlighting);
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
#include "common_tools.fx"
#include "brdf_tools.fx"

#define MAX_DIST 2.0f
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
	int gMODE;

	PointLightBRDF gPointLight;
	MaterialBRDF   gInterMat;
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

//--------------------------
//2d texture
//---------------------------
Texture2D gTexture;

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

	while(dist<MAX_DIST&& all(accum<1.0f) )
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

//ray march to light
float4 ShadowConeTracing(float3 dir,float3 startPos, float3 endPos, float cone_ratio,float4 mat)
{
	float acc = 0;

	float dist = gVoxelSize;
	float STOP = length(endPos-startPos);

	while(STOP>=0 && dist <= STOP &&acc<1.0f)
	{

		float diameter = cone_ratio*dist;
		float lodLevel = diameter;

		float3 ray = startPos + dist * dir;
		float3 ray_svo = world_to_svo(ray,gVoxelSize,gVoxelOffset);

		if(!(all(ray_svo> 0) && all(ray_svo < gDim))) 
			break;		

		float color = gVoxelList.SampleLevel(SVOFilter,ray_svo/gDim + 0.5f / gDim, dist).a;

		acc += (1 - acc)*color;

		dist += gVoxelSize;
	}

	acc *= PI / 2.0f;
	return (1 - acc) ;
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

float4 SpecularCone(float3 dir, float3 startpos,float cone_ratio, MaterialBRDF mat)
{
	float4 accum = 0.0f;

	float dist = gVoxelSize;

	// Trace.
	while (dist < MAX_DIST) {

		float3 ray = startpos + dir*dist;
		float3 ray_svo = world_to_svo(ray, gVoxelSize, gVoxelOffset);

		if (!(all(ray_svo > 0) && all(ray_svo < gDim)))
			break;

		float diameter = cone_ratio*dist;
		float lodLevel = log2(diameter / gVoxelSize);

		float4 color = gVoxelList.SampleLevel(SVOFilter, ray_svo / gDim + 0.5f / gDim, lodLevel);

		float a = 1.0f - accum.a;
		accum += color*a;

		dist += gVoxelSize;
	}
	return float4(pow((mat.metallic + 1 - mat.roughness), 1.2) * accum.rgb, 1.0f);
}

float4 IndirectLighting(float3 N,float3 V,float3 posW,MaterialBRDF mat )
{
	float4 diff = DiffuseCone(N, posW)*float4(mat.albedo,1.0f);

	//TODO modify specular part
	float3 specdir = normalize(reflect(V, N));
	float4 spec = SpecularCone(specdir, posW+ N *gVoxelSize*1.414 * 2, 0.2633f, mat);
	return diff + spec;
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
//MODE 1 : VOXELIZATION VISUALIZE
//-------------------------
float4 PS_Direct(VS_OUT pin) : SV_Target
{
	float3 N = normalize(pin.normW);
	float3 V = normalize(gEyePosW - pin.posW.xyz);
	float4 directlighting = 0.0f;
	float4 shadow = 0.0f;
	float4 indirectlighting = 0.0f;

	MaterialBRDF mat;
	setMatBunnyGold(pin.ID,gInterMat, mat);

	for(uint i=0;i<LIGHT_NUM;i++)
	{
		float3 lightVec = gPointLight.position.xyz - pin.posW.xyz;
		float3 L = normalize(lightVec);
		float3 H = normalize(V + L);

		directlighting += DirectLighting(N, H, lightVec, V, L, gPointLight, mat);
	}

	return directlighting;
}


//----------------------------
//MODE 2 : GLOBAL ILLUMINATION
//-------------------------
float4 PS_GI(VS_OUT pin) : SV_Target
{
	float3 N = normalize(pin.normW);
	float3 V = normalize(gEyePosW - pin.posW.xyz);

	float4 directlighting = 0.0f;
	float4 shadow = 0.0f;
	float4 indirectlighting = 0.0f;

	MaterialBRDF mat;
	setMatBunnyGold(pin.ID,gInterMat, mat);

	float3 lightVec = gPointLight.position.xyz - pin.posW.xyz;
	//float3 lightVec = float3(0,1,1);
	float3 L = normalize(lightVec);
	float3 H = normalize(V + L);

	directlighting = DirectLighting(N, H, lightVec, V, L, gPointLight, mat);

	float3 offset = N *gVoxelSize*1.414 * 2;

	indirectlighting = IndirectLighting(N, V, pin.posW.xyz, mat);
	shadow = ShadowConeTracing(L, pin.posW.xyz + offset, gPointLight.position.xyz, 2.0f / 1.732f, indirectlighting);
	return (directlighting*shadow + indirectlighting);
}

//----------------------------
//CARTOON SHADING
//-------------------------

VS_OUT VS_Car(VS_IN vin)
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

float4 PS_Car(VS_OUT pin) : SV_Target
{
	float3 N = normalize(pin.normW);

	float3 L = normalize(gPointLight.position.xyz - pin.posW.xyz);

	float diffuse = dot(L, N);

	if (diffuse > 0.8) {
		diffuse = 1.0;
	}
	else if (diffuse > 0.5) {
		diffuse = 0.6;
	}
	else if (diffuse > 0.2) {
		diffuse = 0.4;
	}
	else {
		diffuse = 0.2;
	}

	MaterialBRDF mat;
	setMatBunnyGold(pin.ID, gInterMat, mat);

	float3 col = gInterMat.albedo*gPointLight.color;

	return float4( col * diffuse, 1.0);
}

RasterizerState SolidRS
{
	FillMode = SOLID;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

RasterizerState WireFrameRS
{
	FillMode = WIREFRAME;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

technique11 ConeTracingTech
{
	pass DirectLightingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Direct()));
		SetRasterizerState(SolidRS);
	}
	pass ConeTracingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_GI()));
		SetRasterizerState(SolidRS);
	}

	pass CartoonShadingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Car()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Car()));
		SetRasterizerState(SolidRS);
	}

	pass WireFramePass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_GI()));
		SetRasterizerState(WireFrameRS);
	}
}
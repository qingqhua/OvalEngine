
#define PI 3.1415926
#define VOXEL_SIZE 1.0f/64.0f
#define DIFFUSE_INDIRECT_FACTOR 0.52f
#define MIPMAP_HARDCAP 5.4f 

#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106

//--------------------------
// direct light structure
//---------------------------
struct PointLight
{
	//float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float Pad;
};


//--------------------------
// material structure
//---------------------------
struct Material
{
	float4 Diffuse;
	float4 Specular;

	//float emissivity;
	//float transparency;
};

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	PointLight gPointLight;
	float4x4 gView;
	float4x4 gProj;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	Material gMat;
};


//--------------------------
//read voxel from 3d texture
//---------------------------
Texture3D<float4> gVoxelList;

//--------------------------
//set sampler
//---------------------------
SamplerState SVOFilter;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL  :NORMAL;
};

struct VS_OUT
{
	float4 posW   : POSITION;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
};

void ComputePointLight(Material mat,PointLight L,float3 pos,float3 normal,float3 toEye,
						out float4 diffuse, out float4 spec)
{
	diffuse=float4 (0,0,0,0);
	spec=float4 (0,0,0,0);

	float3 lightVec=L.Position-pos;

	//distance from light to surface
	float d=length(lightVec);

	if(d>L.Range) return;

	//normalize light vector
	lightVec/=d;

	//diffuse lighting part
	float diffFactor=dot(lightVec,normal);
	if(diffFactor>0.0f)
	{

	diffuse=diffFactor *L.Diffuse*mat.Diffuse;

	//spec lighting part
	float3 r=reflect(-lightVec,normal);
	float specFactor=pow(max(dot(toEye,r),0.0f),mat.Specular.a);
	spec = specFactor * mat.Specular * L.Specular;
	}
	
	float attFactor= 1.0f / dot(L.Attenuation, float3(1.0f, d, d*d));;
	diffuse *= attFactor;
	spec    *= attFactor;
}

// Returns a floattor that is orthogonal to u.
float3 orthogonal(float3 u){
	u = normalize(u);
	float3 v = float3(0.99146, 0.11664, 0.05832); // Pick any normalized floattor.
	return abs(dot(u, v)) > 0.99999f ? cross(u, float3(0, 1, 0)) : cross(u, v);
}

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
float3 scaleAndBias(const float3 p) { return 0.5f * p + float3(0.5f,0.5f,0.5f); }

// Traces a diffuse voxel cone.
float3 traceDiffuseVoxelCone(const float3 from, float3 direction){
	direction = normalize(direction);
	
	const float CONE_SPREAD = 0.325;

	float4 acc = float4(0.0f,0.0f,0.0f,0.0f);

	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.1953125;

	// Trace.
	while(dist < SQRT2 && acc.a < 1){
		float3 c = from + dist * direction;
		c = scaleAndBias(from + dist * direction)+128.0f;
		float l = (1 + CONE_SPREAD * dist / VOXEL_SIZE);
		float level = log2(l);
		float ll = (level + 1) * (level + 1);
		//todo voxel check
		uint3 pos = uint3(c);
		float4 voxel = gVoxelList.SampleLevel(SVOFilter, pos, min(MIPMAP_HARDCAP, level));
		//float4 voxel =float4(pos,0);
		acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
		dist += ll * VOXEL_SIZE * 2;
	}
	return pow(acc.rgb * 2.0, float3(1.5f,1.5f,1.5f));
}

float3 IndirectDiffLighting(float3 pos,float3 normal,Material material)
{
	const float ANGLE_MIX=0.5f;
	const float w[3]={1.0,1.0,1.0};

	const float3 ortho = normalize(orthogonal(normal));
	const float3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	const float3 corner = 0.5f * (ortho + ortho2);
	const float3 corner2 = 0.5f * (ortho - ortho2);

	//todo voxel
	const float3 N_OFFSET = normal * (1 + 4 * ISQRT2) * VOXEL_SIZE;
	const float3 C_ORIGIN = pos + N_OFFSET;

	// Accumulate indirect diffuse light.
	float3 acc = float3(0,0,0);

	// We offset forward in normal direction, and backward in cone direction.
	// Backward in cone direction improves GI, and forward direction removes
	// artifacts.
	const float CONE_OFFSET = -0.01;

	// Trace front cone
	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal);

	// Trace 4 side cones.
	const float3 s1 = lerp(normal, ortho, ANGLE_MIX);
	const float3 s2 = lerp(normal, -ortho, ANGLE_MIX);
	const float3 s3 = lerp(normal, ortho2, ANGLE_MIX);
	const float3 s4 = lerp(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

	// Trace 4 corner cones.
	const float3 c1 = lerp(normal, corner, ANGLE_MIX);
	const float3 c2 = lerp(normal, -corner, ANGLE_MIX);
	const float3 c3 = lerp(normal, corner2, ANGLE_MIX);
	const float3 c4 = lerp(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

	// Return result.
	return DIFFUSE_INDIRECT_FACTOR * material.Diffuse.a * acc * (material.Diffuse.xyz + float3(0.001f,0.001f,0.001f));

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

	vout.normW=mul(float4(vin.normL,1.0f),gWorld).xyz;
	return vout;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	float3 viewVec=normalize(pin.posW.xyz-gEyePosW);

	float4 rgba=float4(IndirectDiffLighting(pin.posW.xyz,pin.normW,gMat),1.0f);
	return rgba;

	//pin.normW=normalize(pin.normW);
	//float3 toEyeW=normalize(gEyePosW-pin.posW.xyz);

	//float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//ComputePointLight(gMat, gPointLight,
	//				pin.posW.xyz,pin.normW, toEyeW,
	//				diffuse,specular);

	//float4 litColor=diffuse+specular;
	//litColor.a=gMat.Diffuse.a;
}

RasterizerState SolidRS
{
	FillMode = SOLID;
	CullMode = BACK;
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
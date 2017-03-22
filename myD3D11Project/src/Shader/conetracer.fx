#include "tools.fx"

//--------------------------
// direct light structure
//---------------------------
struct PointLight
{
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

float3 DirectDiffuseBRDF(float3 diffAlbedo,float nDotL)
{
	return (diffAlbedo*nDotL)/PI;
}

float3 DirectSpecularBRDF(float3 specAlbedo,float3 posW,float3 normW,float3 lightDir,float roughness)
{
	float3 viewDir=normalize(gEyePosW-posW);
	float3 halfVec=normalize(viewDir+lightDir);

	float nDotH=saturate(dot(normW,halfVec));
	float nDotL=saturate(dot(normW,lightDir));
	float nDotV=max(dot(normW,viewDir),0.0001f);

	float alpha2=roughness*roughness;

	float D=alpha2/(PI*pow(nDotH*nDotH*(alpha2-1)+1,2.0f));

	float3 F= Schlick_Fresnel(specAlbedo, halfVec,lightDir);

	float G=G_Smith(roughness,nDotV,nDotL);

	return D*F*G;
}

float3 DirectLighting(float3 normW,float3 posW,float3 lightColor, float3 lightVec,float3 diffAlbedo, float3 specAlbedo,float roughness)
{
	float3 dist=length(lightVec);
	lightVec/=dist;

	float nDotL=saturate(dot(normW,lightVec));

	float3 directLighting=0.0f;

	if(nDotL>0.0f)
		directLighting = DirectDiffuseBRDF(diffAlbedo,nDotL)+
						DirectSpecularBRDF(specAlbedo,posW,normW,lightVec,roughness);

	return max(directLighting,0.0f)*lightColor;
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
	float3 normW=normalize(pin.normW);
	float3 lightVec=gPointLight.Position-gEyePosW;

	float3 diffAlbedo=gMat.Diffuse.xyz;
	float3 specAlbedo=gMat.Specular.xyz;
	float roughness=gMat.Diffuse.a;
	float nDotL=dot(normW,lightVec);

	float3 directLighting=DirectLighting(normW,pin.posW.xyz,gPointLight.Attenuation, lightVec,diffAlbedo,specAlbedo,roughness);
	return float4(directLighting,1.0f);
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
#include "light.fx"

#define PI 3.1415926

//--------------------------------
//cook-torrance lighting model
//reference: https://learnopengl.com/#!PBR/Theory
//-------------------------------

//Normal Distribution Function (NDF)
//GGX Trowbridge-Reitz
float D_GGX(float3 N,float3 H, float roughness)
{
	float a=roughness*roughness;
	float a2=a*a;

	float NdotH=max(dot(N,H),0.0f);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

//--------------------------------
//geometric shadowing
//-------------------------------
float G_SchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float G_Smith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = G_SchlickGGX(NdotV, roughness);
    float ggx1 = G_SchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

//--------------------------------
//Fresnel factor using Schlickfs approximation
//means the amount of light that reflects from a mirror surface
//fo = specAlbedo
//-------------------------------
float3 Schlick_Fresnel(float3 f0, float3 H, float3 V)
{
	float cosTheta=max(dot(H, V), 0.0);
	return f0 + (1.0f - f0) * pow((1.0f - cosTheta), 5.0f);
}

//--------------------------
//direct diffuse lighting
//lambert: c/PI
//---------------------------
float3 DirectDiffuseBRDF(MaterialBRDF _mat)
{
	return _mat.diffAlbedo/PI;
}

//--------------------------
//direct specular lighting
//  NDF * G * F / denominator
//---------------------------
void DirectSpecularBRDF(float3 N,float3 H,float3 L,float3 V, MaterialBRDF _mat,
						out float3 color,out float3 F)
{    
	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)    
	float3 metallic=_mat.metallic;

	float3 f0 = 0.04f; 
    f0 = lerp(f0, _mat.diffAlbedo, metallic);

	F = Schlick_Fresnel(f0,H,V);
	float NDF = D_GGX(N, H, _mat.roughness);   
    float G = G_Smith(N, V, L, _mat.roughness);      

	float3 nominator = NDF * G * F; 
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;

	color = nominator / denominator;
}

//--------------------------
//BRDF
//compute spec and diff direct lighting
//---------------------------
float4 DirectLighting(float3 N,float3 H,float3 lightVec,float3 V, float3 L,PointLightBRDF _light, MaterialBRDF _mat)
{
	float dist=length(lightVec);
	float att=5.0f/(dist*dist);
	float3 radiance=_light.color*att;

	float3 kS=0.0f;
	float3 specBRDF=0.0f;

	DirectSpecularBRDF(N,H,L,V,_mat, 
	//out 
	specBRDF,kS);

	float3 kD = 1.0f - kS;
    kD *= 1.0f - _mat.metallic;	
	  
	float3 diffBRDF=DirectDiffuseBRDF(_mat);

	float NdotL = max(dot(N, L), 0.0);   
	float3 Lo = (kD * diffBRDF + specBRDF) * radiance * NdotL;

	float3 ambient = 0.03 * _mat.diffAlbedo * float3(0.8,0.8,0.8);

	float3 color=Lo+ambient;
	
    //HDR tonemapping
    color = color / (color + 1.0);
    //gamma correct
    color = pow(color, 1.0/2.2); 

	return float4(color,1.0f);
}

//----------------------------
//compute local point light color
//-------------------------
void BlinnPointLight(float3 pos,float3 normal,float3 toEye, PointLightBRDF _light, MaterialBRDF _mat,
						out float4 diffuse, out float4 spec)
{
	diffuse=float4 (0,0,0,0);
	spec=float4 (0,0,0,0);

	normal=normalize(normal);

	float3 lightVec=_light.position-pos;

	//distance from light to surface
	float d=length(lightVec);

	//normalize light vector
	lightVec=normalize(lightVec);

	//diffuse lighting part
	float diffFactor=dot(lightVec,normal);
	if(diffFactor>0.0f)
	{

	diffuse=float4(diffFactor *_light.color*_mat.diffAlbedo,1.0f);

	//spec lighting part
	float3 r=reflect(-lightVec,normal);
	float specFactor=pow(max(dot(toEye,r),0.0f),_mat.roughness);
	spec = float4(specFactor * _mat.specAlbedo * _light.color,1.0f);
	}

	float att=0.01f;
	float attFactor= 1.0f / dot(att, float3(1.0f, d, d*d));;
	diffuse *= attFactor;
	spec    *= attFactor;
}


//-----------------------------------------------------------------------------------------
// Default Sampler.
//-----------------------------------------------------------------------------------------
SamplerState SVOFilter
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
};

//-----------------------------------------------------------------------------------------
// give offset to transfer to voxel coordinate
//-----------------------------------------------------------------------------------------
uint3 world_to_svo(float3 posW,float voxel_dim)
{
	return ceil(posW+voxel_dim/2.0f);
}

//-----------------------------------------------------------------------------------------
// map from -1,1 to 0,1
//-----------------------------------------------------------------------------------------
float map(float from)
{
	return float(0.5f*from+0.5f);
}
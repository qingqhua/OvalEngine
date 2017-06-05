#include "light_brdf.fx"

#define PI 3.1415926

//--------------------------------
//cook-torrance lighting model
//reference: https://learnopengl.com/#!PBR/Theory
//-------------------------------

//Normal Distribution Function (NDF)
//GGX Trowbridge-Reitz
float D_GGX(float3 N, float3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;

	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
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

	float nom = NdotV;
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
//Fresnel factor using Schlicks approximation
//means the amount of light that reflects from a mirror surface
//fo = specAlbedo
//-------------------------------
float3 Schlick_Fresnel(float3 f0, float cosTheta)
{
	return f0 + (1.0f - f0) * pow((1.0f - cosTheta), 5.0f);
}

//--------------------------
// direct specular lighting
// NDF * G * F / denominator
//---------------------------
float3 DirectSpecularBRDF(float3 N, float3 H, float3 L, float3 V, MaterialBRDF _mat)
{
	float3 f0 = lerp(0.04, _mat.albedo, _mat.metallic);

	float3 F = Schlick_Fresnel(f0, max(dot(H, V), 0.0));
	float NDF = D_GGX(N, H, _mat.roughness);
	float G = G_Smith(N, V, L, _mat.roughness);

	float3 nominator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f;

	return nominator / denominator;
}

//--------------------------
//BRDF
//compute spec and diff direct lighting
//---------------------------
float4 DirectLighting(float3 N, float3 H, float3 lightVec, float3 V, float3 L, PointLightBRDF _light, MaterialBRDF _mat)
{
	float3 f0 = lerp(0.04, _mat.albedo, _mat.metallic);

	float dist = length(lightVec);
	float attenuation = 1.0f / (dist * dist);
	float3 radiance = _light.color * attenuation;

	float3 kS = Schlick_Fresnel(f0, max(dot(H, V), 0.0));

	float3 kD = 1.0 - kS;
	kD*=1.0-_mat.metallic;

	float3 specBRDF = DirectSpecularBRDF(N, H, L, V, _mat);
	float3 diffBRDF = (kD*_mat.albedo / PI);

	float NdotL = max(dot(N, L), 0.0);
	float3 Lo = (diffBRDF + specBRDF) * radiance * NdotL;
	
	return float4(Lo, 1.0f);
}

//----------------------------------------------
// tone mapping
//----------------------------------------------
float3 ACESToneMapping(float3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

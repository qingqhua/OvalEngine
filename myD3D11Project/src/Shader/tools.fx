
#define PI 3.1415926

//--------------------------------
//cook-torrance lighting model
//reference: https://learnopengl.com/#!PBR/Theory
//-------------------------------
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

float3 Schlick_Fresnel(float3 f0, float3 H, float3 V)
{
	float cosTheta=max(dot(H, V), 0.0);
	return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}


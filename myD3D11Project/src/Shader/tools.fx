#define PI 3.1415926

float GGX(float NdotV, float a)
{
	float k = a / 2;
	return NdotV / (NdotV * (1.0f - k) + k);
}

float G_Smith(float a, float nDotV, float nDotL)
{
	return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}

float3 Schlick_Fresnel(float3 f0, float3 h, float3 l)
{
	return f0 + (1.0f - f0) * pow((1.0f - dot(l, h)), 5.0f);
}


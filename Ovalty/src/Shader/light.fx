//--------------------------
// direct light structure
//---------------------------
struct PointLightBRDF
{
	float3 position;
	float3 color;
};

//--------------------------
// material structure
//---------------------------
struct MaterialBRDF
{
	float3 diffAlbedo;
	float metallic;
	float3 specAlbedo;
	float roughness;
};
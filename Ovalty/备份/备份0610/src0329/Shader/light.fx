#define LIGHT_NUM 2
//--------------------------
// direct light structure
//---------------------------
struct PointLightBRDF
{
	float3 position;
	float3 color;
	float intensity;
};

void setPointLight(out PointLightBRDF L1,out PointLightBRDF L2)
{
	L1.position=float3(-0.5f, 0.5f, -1.0f);
	L1.color=float3(1.0f, 1.0f, 1.0f);
	L1.intensity=1.0f;

	L2.position=float3(2225.5f, 1.0f, -1.0f);
	L2.color=float3(1.0f, 1.0f, 1.0f);
	L2.intensity=1.0f;
}

//--------------------------
// material structure
//---------------------------
struct MaterialBRDF
{
	float3 albedo;
	float metallic;
	float roughness;
};

//--------------------------
// set material attribute
//---------------------------
void setMatSilver(out MaterialBRDF M)
{
	M.albedo = float3(0.972,0.960,0.915);
	M.metallic = 0.0f;
	M.roughness = 0.3f;
}

void setMatCopper(out MaterialBRDF M)
{
	M.albedo = float3(0.955,0.638,0.538);
	M.metallic = 0.0f;
	M.roughness = 0.3f;
}

void setMatGold(out MaterialBRDF M)
{
	M.albedo = float3(1.022,0.782,0.344);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.5f;
	M.roughness = 0.3f;
}

void setMatPerObject(int id,out MaterialBRDF mat)
{
	//sphere
	if(id<3264)
	 setMatSilver(mat);
	//short box
	else if(id>=3264&&id<3300)
		  setMatCopper(mat);
	//floor
	else if(id>=3300&&id<3306)
		 setMatSilver(mat);
	//ceiling
	else if(id>=3306&&id<3312)
		 setMatSilver(mat);
	//backwall
	else if(id>=3312&&id<3318)
		 setMatCopper(mat);
	//leftwall
	else if(id>=3324&&id<3330)
		 setMatCopper(mat);
	//rightwall
	else if(id>=3300&&id<3336)
		 setMatSilver(mat);
}

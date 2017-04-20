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

void setPointLight(out PointLightBRDF L1,out PointLightBRDF L2, float t)
{
	//L1.position=float3(-0.0f, 1.0f, 0.0f);
	
	L1.position = float3(0.5*cos(t), 1.2,  -1.0f+0.5*sin(t));
	L1.color=float3(1.0f, 1.0f, 1.0f);
	L1.intensity=1.0f;

	L2.position=float3(-0.0f, 100.8f, 0.0f);
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
	float transparency;
};

//--------------------------
// set material attribute
//---------------------------
void setMatSilver(out MaterialBRDF M)
{
	M.albedo = float3(0.972,0.960,0.915);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=1.0f;
}

void setMatCopper(out MaterialBRDF M)
{
	M.albedo = float3(0.955,0.638,0.538);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=0.0f;
}

void setMatGoldCopper(out MaterialBRDF M)
{
	M.albedo = float3(0.955, 0.638, 0.538);
	M.metallic = 0.7f;
	M.roughness = 0.6f;
	M.transparency = 0.0f;
}

void setMatGreen(out MaterialBRDF M)
{
	M.albedo = float3(0.0,0.824,0.850);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=1.0f;
}

void setMatGold(out MaterialBRDF M)
{
	M.albedo = float3(1.022,0.782,0.344);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=1.0f;
}

void setMatWhite(out MaterialBRDF M)
{
	M.albedo = float3(1.000,1.00,1.00);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
	M.transparency=1.0f;
}

void setMatCornellBox(int id,out MaterialBRDF mat)
{
	//sphere
	if (id < 3264)
		setMatCopper(mat);
	//short box
	else if (id >= 3264 && id < 3300)
		setMatGold(mat);
	//floor
	else if (id >= 3300 && id < 3306)
		setMatWhite(mat);
	//ceiling
	else if (id >= 3306 && id < 3312)
		setMatCopper(mat);
	//backwall
	else if (id >= 3312 && id < 3318)
		setMatWhite(mat);
	//leftwall
	else if (id >= 3324 && id < 3330)
		setMatGreen(mat);
	//rightwall
	else if (id >= 3300 && id < 3336)
		setMatGold(mat);
}

#define LIGHT_NUM 2
//--------------------------
// direct light structure
//---------------------------

struct PointLightBRDF
{
	float4 position;
	float3 color;
};

//set light in gpu
void setPointLightBRDF(out PointLightBRDF L, float t)
{
	//L.position=float3(0.5f, 1.0f, -1.0f);
	
	L.position = float4(0.5*cos(t), 1.2, -0.5f + 0.5*sin(t), 1.0f);
	L.color=float3(1.0f, 1.0f, 1.0f);
}

void setPointLight2BRDF(out PointLightBRDF L, out PointLightBRDF L2, float t)
{
	//L.position=float3(0.5f, 1.0f, -1.0f);

	L.position = float4(0.5*cos(t), 1.2, -0.5f + 0.5*sin(t), 1.0f);
	L.color = float3(1.0f, 1.0f, 1.0f);

	L2.position = float4(0.5 + 0.5*sin(t), 1.2*cos(t), -0.5f , 1.0f);
	L2.color = float3(1.0f, 1.0f, 1.0f);
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
// set material in gpu
//---------------------------
void setMatSilver(out MaterialBRDF M)
{
	M.albedo = float3(0.972,0.960,0.915);
	M.metallic = 1.0f;
	M.roughness = 0.0f;
}

void setMatCopper(out MaterialBRDF M)
{
	M.albedo = float3(0.955,0.338,0.338);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
}

void setMatGreen(out MaterialBRDF M)
{
	M.albedo = float3(0.3,0.924,0.350);
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatGold(out MaterialBRDF M)
{
	M.albedo = float3(1.0f,241.0f/ 256.0f,0.0);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.3f;
	M.roughness = 0.3f;
}

void setMatWhite(out MaterialBRDF M)
{
	M.albedo = float3(1.000,1.00,1.00);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatBlue(out MaterialBRDF M)
{
	M.albedo = float3(0.000, 0.00, 1.00);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatYellow(out MaterialBRDF M)
{
	M.albedo = float3(1.000, 1.00, 0.00);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatPurple(out MaterialBRDF M)
{
	M.albedo = float3(153.0f/255.0f, 51.0f/255.0f, 250.00f/255.0f);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatOrange(out MaterialBRDF M)
{
	M.albedo = float3(231.0f / 255.0f, 101.0f / 255.0f, 26.00f / 255.0f);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.0f;
	M.roughness = 1.0f;
}

void setMatBunnyGold(int id, MaterialBRDF interactiveMat, out MaterialBRDF mat)
{

	if (id > 32 &&id <= 32+7224)
		mat = interactiveMat;
	else if (id > 32 + 7224 && id <= 32 + 7224 + 3600 * 9)
		setMatBlue(mat);
	else if (id > 32 + 7224 + 3600 * 9 && id < 32 + 7224 + 3600 * 15)
		setMatCopper(mat);
	else if(id > 32 + 7224 + 3600 * 15 && id < 32 + 7224 + 3600 * 19)
		setMatWhite(mat);
	else if (id > 32 + 7224 + 3600 * 19 && id < 32 + 7224 + 3600 * 23)
		setMatOrange(mat);
	else if (id > 32 + 7224 + 3600 * 23 && id < 32 + 7224 + 3600 * 30)
		setMatPurple(mat);
	else if (id > 32 + 7224 + 3600 * 30 && id < 32 + 7224 + 3600 * 35)
		setMatYellow(mat);
	else setMatWhite(mat);
}


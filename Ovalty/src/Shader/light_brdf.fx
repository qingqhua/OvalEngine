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

void setPointLightBRDF(out PointLightBRDF L1,out PointLightBRDF L2, float t)
{
	//L1.position=float3(0.5f, 1.0f, -1.0f);
	
	L1.position = float3(0.5*cos(t), 1.2,  -0.5f+0.5*sin(t));
	L1.color=float3(1.0f, 1.0f, 1.0f);
	L1.intensity=1.0f;

	L2.position=float3(2.0f, 2.8f, -3.0f);
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
	M.albedo = float3(0.955,0.338,0.338);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=0.0f;
}

void setMatGreen(out MaterialBRDF M)
{
	M.albedo = float3(0.3,0.924,0.350);
	M.metallic = 0.0f;
	M.roughness = 0.0f;
	M.transparency=1.0f;
}

void setMatGold(out MaterialBRDF M)
{
	M.albedo = float3(1.0f,241.0f/ 256.0f,0.0);
	//if metallic = 0, we have no specular
	//if metallic = 1, we have no diffuse
	M.metallic = 0.3f;
	M.roughness = 0.3f;
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
	int bunny_index = 14934;
	if (id < bunny_index - 32)
	{
		setMatGold(mat);
	}
	//floor
	else if (id >= bunny_index - 32 &&id < bunny_index - 32 + 6)
		setMatWhite(mat);
	//ceiling
	else if (id >= bunny_index - 32 + 6 && id < bunny_index - 32 + 12)
		setMatWhite(mat);
	//backwall
	else if (id >= bunny_index - 32 + 12 && id < bunny_index - 32 + 18)
		setMatWhite(mat);
	//right wall
	else if (id >= bunny_index - 32 + 18 && id < bunny_index - 32 + 24)
		setMatCopper(mat);
	//left wall
	else if (id >= bunny_index - 32 + 24 && id < bunny_index - 32 + 32)
		setMatGreen(mat);
	else 
		setMatGold(mat);
}

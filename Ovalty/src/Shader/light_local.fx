struct PointLightLocal
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct MaterialLocal
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

void setPointLightLocal(out PointLightLocal L, float t)
{
	L.Position = float3(0.8f, 1.0f, -1.0f);

	L.Ambient = float4(0.0f, 0.0f, 0.0f,1.0f);
	L.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	L.Specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	L.Range = 5.0f;

	L.Att = float3(-0.0f, 0.1f, 0.8f);
	L.pad = 1.0f;
}

//--------------------------
// set material attribute
//---------------------------
void setMatWhite(out MaterialLocal M)
{
	M.Ambient = float4(1.0f, 1.0f, 1.0f,1.0f);
	M.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	M.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	M.Reflect = float4(1.0f, 1.0f, 1.0f, 1.0f);
}

void setMatCopper(out MaterialLocal M)
{
	M.Ambient = float4(0.955, 0.338, 0.338, 1.0f);
	M.Diffuse = float4(0.955, 0.338, 0.338, 1.0f);
	M.Specular = float4(0.955, 0.338, 0.338, 1.0f);
	M.Reflect = float4(0.955, 0.338, 0.338, 1.0f);
}

void setMatGreen(out MaterialLocal M)
{
	M.Ambient = float4(0.3, 0.924, 0.350, 1.0f);
	M.Diffuse = float4(0.3, 0.924, 0.350, 1.0f);
	M.Specular = float4(0.3, 0.924, 0.350, 1.0f);
	M.Reflect = float4(0.3, 0.924, 0.350, 1.0f);
}

void setMatGold(out MaterialLocal M)
{
	M.Ambient = float4(1.0f, 241.0f / 256.0f, 0.0,1.0f);
	M.Diffuse = float4(1.0f, 241.0f / 256.0f, 0.0, 1.0f);
	M.Specular = float4(1.0f, 241.0f / 256.0f, 0.0, 1.0f);
	M.Reflect = float4(1.0f, 241.0f / 256.0f, 0.0, 1.0f);
}



void setMatCornellBox(int id,out MaterialLocal mat)
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


//---
//modify from d3dcoder.net
//---
void ComputePointLight(MaterialLocal mat, PointLightLocal L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec *= att;
}
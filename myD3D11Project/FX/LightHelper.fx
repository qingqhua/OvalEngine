struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; 
	float4 Reflect;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{
	float4 Amibent;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float Pad;
};

struct SpotLight
{
	float4 Amibent;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float Pad;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L,float3 normal, float3 toEye,
							out float4 ambient,out float4 diffuse,out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = -L.Direction;

	// ambient term.
	ambient = mat.Ambient * L.Ambient;
	
	//diffuse term
	float diffuseFactor = max(0,dot(lightVec, normal));
	diffuse = diffuseFactor*mat.Diffuse*L.Diffuse;

	//spec term

}


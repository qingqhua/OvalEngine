struct DirLight
{
	float4 diffuse;
	float4 ambient;
	float4 specular;

	float3 dir;
};

struct material
{
	float4 diffuse;
	float4 ambient;
	float4 specular;
};

struct Wall
{

	float4 color;
};

cbuffer cbPerObject
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	DirLight dirLight;
	float3 eyePos;
	material matBox;
	material matBackWall;
	material matFloor;
};


RasterizerState WireframeRS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

struct VertexIn
{
	float3 pos  : POSITION;
	float3 norm : NORMAL;
};

struct VertexOut
{
	float4 pos  : SV_POSITION;
	float3 norm : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.pos = mul(float4(vin.pos, 1.0f), worldMatrix);
	vout.pos = mul(vout.pos, viewMatrix);
	vout.pos = mul(vout.pos, projMatrix);

	vout.norm = mul(float4(vin.norm,1), worldMatrix).xyz;
	vout.norm = normalize(vout.norm);

	return vout;
}

float4 PS_Box(VertexOut pin) : SV_Target
{
	float4 color;
	float3 I = dirLight.dir;
	float3 L = -I;

	//diffuse part
	float diffFactor= max(0,(dot(L, pin.norm)));
	float4 diffuse = diffFactor*matBox.diffuse*dirLight.diffuse;

	//ambient part
	float4 ambient = dirLight.ambient*matBox.ambient;

	//specular part
	float3 r = I - 2 * (dot(pin.norm, I)*I);
	float3 toEye = normalize(eyePos - pin.pos.xyz);
	float specFactor = pow(saturate(dot(toEye, r)), matBox.specular.w);
	float4 specular = specFactor*dirLight.specular*matBox.specular;

	color = diffuse + ambient + specular;
	color.a = matBox.diffuse.a;
	return color;
}


float4 PS_BackWall(VertexOut pin) : SV_Target
{
	float4 color;
	float3 I = dirLight.dir;
	float3 L = -I;

	//diffuse part
	float diffFactor = max(0,(dot(L, pin.norm)));
	float4 diffuse = diffFactor*matBackWall.diffuse*dirLight.diffuse;

	//ambient part
	float4 ambient = dirLight.ambient*matBackWall.ambient;

	//specular part
	float3 r = I - 2 * (dot(pin.norm, I)*I);
	float3 toEye = normalize(eyePos - pin.pos.xyz);
	float specFactor = pow(saturate(dot(toEye, r)), matBackWall.specular.w);
	float4 specular = specFactor*dirLight.specular*matBackWall.specular;

	color = diffuse + ambient + specular;
	color.a = matBackWall.diffuse.a;
	return color;
}

float4 PS_Floor(VertexOut pin) : SV_Target
{
	float4 color;
	float3 I = dirLight.dir;
	float3 L = -I;

	//diffuse part
	float diffFactor = max(0,(dot(L, pin.norm)));
	float4 diffuse = diffFactor*matFloor.diffuse*dirLight.diffuse;

	//ambient part
	float4 ambient = dirLight.ambient*matFloor.ambient;

	//specular part
	float3 r = I - 2 * (dot(pin.norm, I)*I);
	float3 toEye = normalize(eyePos - pin.pos.xyz);
	float specFactor = pow(saturate(dot(toEye, r)), matFloor.specular.w);
	float4 specular = specFactor*dirLight.specular*matFloor.specular;

	color = diffuse + ambient + specular;
	color.a = matFloor.diffuse.a;
	return color;
}

technique11 LightTech
{
	pass P_Box
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Box()));
	}


	pass P_BackWall
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_BackWall()));
	}

	pass P_Floor
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Floor()));
	}
}


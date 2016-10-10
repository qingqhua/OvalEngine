struct DirLight
{
	float4 Dir;
	float4 Color;
};

cbuffer cbPerObject
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	DirLight light;
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

float4 PS(VertexOut pin) : SV_Target
{
	float4 color = 0;
	float4 lightDir = -light.Dir;
	color= saturate(dot((float3)light.Dir, pin.norm)*light.Color);
	color.a = 1;
	return color;
}

technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
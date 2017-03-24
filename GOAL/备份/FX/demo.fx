//-----------------------------------
//FILE:demo.fx
//render a simple box with light, material and 2dtexture
//vs gs ps SamplerState RasterizerState DepthStencilState
//-----------------------------------
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

cbuffer cbPerObject
{
	float4x4 worldMatrix;
};

cbuffer cbPerFrame
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
	DirLight dirLight;
	float3 eyePos;
	material mat;
}

Texture2D diffusemap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 1;

	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState SolidRS
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState WireframeRS
{
	FillMode = WireFrame;
	CullMode = None;
	FrontCounterClockwise = false;
};

DepthStencilState DisableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = 0;
};

struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float3 normW : NORMAL;
    float2 tex : TEXCOORD0;
};


VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;
	float4 posW = mul(float4(vin.posL, 1.0f), worldMatrix);
	float4 posV = mul(posW, viewMatrix);
	vout.posH = mul(posV, projMatrix);

	vout.normW = mul(float4(vin.normL,1), worldMatrix).xyz;
	vout.normW = normalize(vout.normW);

	vout.tex = vin.tex;

	return vout;
}

[maxvertexcount(6)]
void GS( triangle VS_OUT input[3], inout TriangleStream<PS_IN> TriStream )
{
    PS_IN output;

    for( uint i=0; i<3; i++ )
    {
        output.posH = input[i].posH;
        output.normW = input[i].normW;
        output.tex = input[i].tex;
        TriStream.Append( output );
    }
	
    TriStream.RestartStrip();

	    for( uint i=0; i<3; i++ )
    {
        output.posH = input[i].posH+10;
        output.normW = input[i].normW;
        output.tex = input[i].tex;
        TriStream.Append( output );
    }
	
    TriStream.RestartStrip();
}

float4 PS(PS_IN pin) : SV_Target
{
	float4 color = 0;
	float4 texcolor = diffusemap.Sample(samAnisotropic, pin.tex);
	if (texcolor.a < 0.1f)
		clip(texcolor.a - 0.1f);

	float3 I = dirLight.dir;
	float3 L = -I;

	//diffuse part
	float diffFactor= max(0,(dot(L, pin.normW)));
	float4 diffuse = diffFactor*mat.diffuse*dirLight.diffuse;

	//ambient part
	float4 ambient = dirLight.ambient*mat.ambient;

	//specular part
	float3 r = I - 2 * (dot(pin.normW, I)*I);
	float3 toEye = normalize(eyePos - pin.posH.xyz);
	float specFactor = pow(saturate(dot(toEye, r)), mat.specular.w);
	float4 specular = specFactor*dirLight.specular*mat.specular;

	color = texcolor*(diffuse + ambient) + specular;
	color.a = mat.diffuse.a*texcolor.a;
	color = (diffuse + ambient) + specular;
	color.a = mat.diffuse.a;
	return color;
}

technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(SolidRS);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
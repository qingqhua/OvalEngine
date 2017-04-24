

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_IN
{
    float3 posL  : POSITION;
	float3 normL : NORMAL;
    float2 tex   : TEXCOORD;
};

struct VS_OUT
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};


VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;

	vout.posH = mul(float4(vin.posL,1.0f), worldMatrix);
	vout.posH = mul(vout.posH, viewMatrix);
	vout.posH = mul(vout.posH, projectionMatrix);
    
	vout.normW= mul(float4(vin.normL, 1.0f), worldMatrix).xyz;

    vout.tex = vin.tex;
    
    return vout;
}
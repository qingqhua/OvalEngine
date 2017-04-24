
struct PS_IN
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};


float4 ps_main(PS_IN pin) : SV_TARGET
{
    return float4(pin.normW,1.0f);
}

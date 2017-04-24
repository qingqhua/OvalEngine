struct GS_IN
{
    float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
    float2 tex   : TEXCOORD;
};

struct GS_OUT
{
	float4 posH  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

[maxvertexcount(3)]
void gs_main(triangle GS_IN gin[3], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT gout;

	for (uint i = 0; i < 3; i++)
	{
		gout.posH = gin[i].posH+5.0f;
		gout.normW = gin[i].normW;
		gout.tex = gin[i].tex;

		triStream.Append(gout);
	}

	triStream.RestartStrip();
}
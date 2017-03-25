cbuffer cbPerObject
{
	float4x4 gWorld;
};

cbuffer cbPerFrame
{
	float4x4 gView;
	float4x4 gProj;
}


struct VS_IN
{
	uint vertexId : SV_VertexID;
};

struct VS_OUT
{
	float4 pos  : SV_POSITION;
};


VS_OUT VS(VS_IN input)
{
	VS_OUT output;

if (input.vertexId == 0)
        output.pos = float4(0.8, 0.5, 0.5, 1.0);
    else if (input.vertexId == 1)
        output.pos = float4(0.5, -0.5, 0.5, 1.0);
    else if (input.vertexId == 2)
        output.pos = float4(-0.5, 0.0, 1.0, 1.0);

	return output;
}

//-----------------------------
//GEOMETRY SHADER
//-----------------------------
[maxvertexcount(3)]
void GS(triangle VS_OUT gin[3],inout TriangleStream<VS_OUT> triStream)
{
	float3 p0 = gin[1].pos.xyz - gin[0].pos.xyz;
	float3 p1 = gin[2].pos.xyz - gin[0].pos.xyz;
	float3 normal = cross(p0,p1);
	normal=normalize(abs(normal));
	float c=max(normal.x,max(normal.y,normal.z));

	for (uint i = 0; i < 3; i++)
	{
		VS_OUT output;

		if(c==normal.x){
			output.pos = float4(gin[i].pos.yz, 0, 1);
		} else if (c==normal.y){
			output.pos = float4(gin[i].pos.xz, 0, 1);
		} else {
			output.pos = float4(gin[i].pos.xy, 0, 1);
		}
		triStream.Append(output);
	}
	triStream.RestartStrip();

}

bool isInsideCube(const float3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

float4 PS(VS_OUT pin) : SV_Target
{
	if(!isInsideCube(pin.pos, 1)){
	 return float4(0.0,1.0,0.0,1.0f);}
	 else {
    return float4(1.0f,1.0f,0.0f,1.0f);
	}
}

technique11 VisualTech
{
	pass VisualTech
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
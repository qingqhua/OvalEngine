#include "common_tools.fx"
//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float gVoxelSize;
	float3 gVoxelOffset;
	float gDim;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
};

//--------------------------
//textures
//---------------------------
Texture3D<float4> gVoxelList;
Texture2D gEdge;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	uint index	: SV_VertexID;
};

struct VS_OUT
{
	uint isvoxel	:texcoord;
	float3 posL		:POSITION;
	float3 normW	: NORMAL;
};

struct PS_IN
{
	float4 pos			: SV_POSITION;
	float3 normW		: NORMAL;	
	float2 texcoord		: TEXCOORD;	
};

static const float2 boxTexArray[4] =
{
	0, 0,
	0, 1,
	1, 0,
	1, 1
};

static const float3 boxOffset[24] =
{
	1, -1, 1,
	1, 1, 1,
	-1, -1, 1,
	-1, 1, 1,

	-1, 1, 1,
	-1, 1, -1,
	-1, -1, 1,
	-1, -1, -1,

	1, 1, -1,
	1, 1, 1,
	1, -1, -1,
	1, -1, 1,

	-1, 1, -1,
	1, 1, -1,
	-1, -1, -1,
	1, -1, -1,

	1, 1, 1,
	1, 1, -1,
	-1, 1, 1,
	-1, 1, -1,

	-1, -1, -1,
	1, -1, -1,
	-1, -1, 1,
	1, -1, 1
};

static const float3 boxNormalArray[6] =
{
	0, 0, 1,
	-1, 0, 0,
	1, 0, 0,
	-1, 0, 0,
	0, 1, 0,
	0, -1, 0
};

//--------------------------------------------------------------------------------------
// Render States.
//--------------------------------------------------------------------------------------
RasterizerState RS_Cull
{
	CullMode = Back;
};

DepthStencilState LessEqualDSS
{
	DepthEnable = TRUE;
	DepthFunc = LESS_EQUAL;
};

//-----------------------------
//VERTEX SHADER
//-----------------------------
VS_OUT VS(VS_IN vin)
{
	float w, h, d;
	gVoxelList.GetDimensions(w, h, d);
	uint VoxelDim = w;
	uint sliceNum = VoxelDim*VoxelDim;

	uint z = vin.index / (sliceNum);
	uint temp = vin.index % (sliceNum);
	uint y = temp / (uint)VoxelDim;
	uint x = temp % (uint)VoxelDim;
	uint3 pos = uint3(x, y, z);

	VS_OUT output;
	if(gVoxelList[pos].w >0)
		output.isvoxel = 1;
	else 
		output.isvoxel = 0;

	output.posL =svo_to_world(pos,gVoxelSize,gVoxelOffset);
	output.posL.x +=3.1f;

	output.normW=gVoxelList[pos].xyz;

	output.normW=gVoxelList.SampleLevel(SVOFilter, pos/gDim+0.5f/gDim ,0);

	return output;
}

//-----------------------------
//GEOMETRY SHADER
//-----------------------------
[maxvertexcount(24)]
void GS(point VS_OUT gin[1],inout TriangleStream<PS_IN> triStream)
{
		if(gin[0].isvoxel){
			// Generate vertexes for six faces.
			for (int i = 0; i < 6; i++)
			{
				// Generate four vertexes for a face.
				for (int j = 0; j < 4; j++)
				{
					PS_IN output;

					//TODO
					float3 vertex = gin[0].posL.xyz+ boxOffset[i * 4 + j] * gVoxelSize;
					
					 //matrix transform
					output.pos = mul(float4(vertex, 1.0f), gWorld);
					output.pos=mul(output.pos, gView);
					output.pos = mul(output.pos, gProj);

					//todo normal offset?
					output.normW=(gin[0].normW);

					output.texcoord = boxTexArray[j];

					triStream.Append(output);

				}
				triStream.RestartStrip();
			}
		}
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(PS_IN pin) : SV_Target
{
	float4 normal = float4(pin.normW, 1);

	float3 output = gEdge.Sample(SVOFilter, pin.texcoord).xyz;
	normal.rgb *= output;

	return float4(pin.normW,1.0);
}

technique11 VisualTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(RS_Cull);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}
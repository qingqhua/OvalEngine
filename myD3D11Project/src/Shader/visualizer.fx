//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gWorld;
	float gVoxelSize;
};

//--------------------------
//textures
//---------------------------
Texture3D gVoxelList;
Texture2D gEdge;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	uint index : SV_VertexID;
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

//-----------------------------------------------------------------------------------------
// Sampler.
//-----------------------------------------------------------------------------------------
SamplerState gAnisotropicSam
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
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
	if(gVoxelList[pos].w)
		output.isvoxel = 1;
	else 
		output.isvoxel = 0;

	output.posL = pos;
	output.normW=gVoxelList[pos].xyz;

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

					float3 vertex = gin[0].posL.xyz+ boxOffset[i * 4 + j] * 0.5f;
					float4x4 translate;
					translate[0] = float4(1,0,0,0);
					 translate[1] = float4(0,1,0,0); 
					 translate[2] = float4(0,0,1,0);
					 translate[3] = float4(-5,-6,0,1);

					 float4x4 rotateZ;
					 rotateZ[0] = float4(cos(90),sin(90),0,0);
					 rotateZ[1] = float4(-sin(90),cos(90),0,0);
					 rotateZ[2] = float4(0,0,1,0);
					 rotateZ[3] = float4(0,0,0,1);

					 float4x4 mat=mul(translate,rotateZ);

					output.pos = mul(float4(vertex*gVoxelSize, 1), mat);
					output.pos=mul(output.pos, gView);
					output.pos = mul(output.pos, gProj);
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

	float3 output = gEdge.Sample(gAnisotropicSam, pin.texcoord).xyz;
	normal.rgb *= output;

	return float4(normal.rgb,1.0);
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
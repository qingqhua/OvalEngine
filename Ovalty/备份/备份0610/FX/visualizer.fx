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

//--------------------------
//voxel shape
//-----------------------
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

static const float2 boxTexArray[4] =
{
	0, 0,
	0, 1,
	1, 0,
	1, 1
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

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	uint index  : SV_VertexID;
};

struct VS_OUT
{
	float3 posL		 : POSITION;
	float3 normal    : NORMAL;
};

struct PS_IN
{
	float4 pos			: SV_POSITION;
	float3 cubenormW	: POSITION;
	float3 voxelnormW	: NORMAL;	
	float2 texcoord		: TEXCOORD;	
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
	// Access a voxel of Texture3D by vertex index.
	float w, h, d;
	gVoxelList.GetDimensions(w, h, d);
	uint VoxelDim = w;

	uint sliceNum = VoxelDim*VoxelDim;
	uint z = vin.index / (sliceNum);
	uint temp = vin.index % (sliceNum);
	uint y = temp / (uint)VoxelDim;
	uint x = temp % (uint)VoxelDim;
	uint3 pos = uint3(x, y, z);

	// Output voxel data to geometry shader.
	VS_OUT output;
	output.posL = pos;
	//output.normal = gVoxelList[pos].xyz;
	output.normal=float3(1.0f,1.0f,1.0f);
	// Decompress normal from 2 components to 3 components.
	half scale = 1.7777;
	half3 nn =
	output.normal*half3(2 * scale, 2 * scale, 0) +
	half3(-scale, -scale, 1);
	half g = 2.0 / dot(nn.xyz, nn.xyz);
	half3 n;
	output.normal.xy = g*nn.xy;
	output.normal.z = g - 1;

	return output;
}

//-----------------------------
//GEOMETRY SHADER
//-----------------------------
[maxvertexcount(24)]
void GS(point VS_OUT gin[1],inout TriangleStream<PS_IN> triStream)
{
		// Only process vertexes with voxel data.
		if (dot(float3(1, 1, 1), gin[0].normal.xyz) > 0.01f)
		{
			// Generate vertexes for six faces.
			for (int i = 0; i < 6; i++)
			{
				// Generate four vertexes for a face.
				for (int j = 0; j < 4; j++)
				{
					PS_IN output;
					// Create cube vertexes with boxOffset array.
					float3 vertex = gin[0].posL.xyz + boxOffset[i * 4 + j] * 0.5f;

					// Output both geometry data for rendering a cube and voxel data for visualization.
					output.pos = mul(float4(vertex*gVoxelSize, 1), gView);
					output.pos = mul(output.pos, gProj);
					output.cubenormW = boxNormalArray[i];
					output.texcoord = boxTexArray[j];
					output.voxelnormW = gin[0].normal;

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
	float4 normal = float4(pin.voxelnormW, 1);

	float3 output = gEdge.Sample(gAnisotropicSam, pin.texcoord);
	normal.rgb *= output;
	return float4(normal);
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
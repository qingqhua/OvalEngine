
#define PI 3.1415926

//-----------------------
//constant buffer
//---------------------

//todo register
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	int gDim;
	float3 gVoxelSize;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
};

//--------------------------
//read write 3d texture
//---------------------------
RWTexture3D<float4> gTargetUAV;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL   : POSITION;
	float3 normL  : NORMAL;
};

struct VS_OUT
{
	float4 posW  : POSW;
	float4 posV  : POSV;
	float3 normW : NORMW;
};

struct PS_IN
{
	float4 pos    : SV_POSITION;	//pos just for rasterization
	float3 normW  : NORMW;
	float3 svoPos : SVO;	//voxel-space boundary coordinates xyz>=0 && xyz<=gDim
};

//--------------------------------------------------------------------------------------
// Render States.
//--------------------------------------------------------------------------------------

// RasterizerState for disabling culling.
RasterizerState RS_CullDisabled
{
	CullMode = None;
};


// BlendState for disabling blending.
BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

// DepthStencilState for disabling depth writing.
DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = 0;
};


//-----------------------------
//VERTEX SHADER
//-----------------------------
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	vout.posW=mul(float4(vin.posL,1.0f),gWorld);
	vout.posV=mul(vout.posW,gView);

	vout.normW=mul(float4(vin.normL,1.0f),gWorld).xyz;

	return vout;
}

//map from -1,1 to 0,1
float map(float from)
{
	return float(0.5f*from+0.5f);
}

//-----------------------------
//GEOMETRY SHADER
//first project each triangle from posW to its main face.
//then transfer its rasterpos to voxel-boundry space. (0-gDim,0-gDim,0-gDim)
//since viewport is the same to gDim, rasterpos is -1,1 map it to voxel-boundry space.
//-----------------------------
[maxvertexcount(3)]
void GS(triangle VS_OUT gin[3],inout TriangleStream<PS_IN> triStream)
{
	float3 facenormal=abs(normalize(cross(gin[1].posW.xyz-gin[0].posW.xyz,gin[2].posW.xyz-gin[0].posW.xyz)));
	float axis=max(facenormal.x,max(facenormal.y,facenormal.z));

	for( uint i = 0; i < 3; i++)
	{
		PS_IN output;

		// Projection the main face
		if (axis == facenormal.x)
		{
			output.pos.xyz = gin[i].posW.zyx;
		}
		else if (axis == facenormal.y)
		{
			output.pos.xyz = gin[i].posW.xzy;
		}
		else output.pos.xyz = gin[i].posW.xyz;
		
		//pos for rasterization
		output.pos.xyz /= (float)gDim;

		//map rasterpos to voxel space
		//todo voxel size
		uint x=map(output.pos.x)*(gDim-1);
		uint y=map(output.pos.y)*(gDim-1);
		uint z=map(output.pos.z)*(gDim-1);

		if (axis == facenormal.x)
		{
			output.svoPos=uint3(z,y,x);
		}
		else if (axis == facenormal.y)
		{
			output.svoPos=uint3(x,z,y);
		}
		else output.svoPos=uint3(x,y,z);

		//pos for rasterization
		output.pos.zw = 1;

		output.normW = gin[i].normW;

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(PS_IN pin) : SV_Target
{
	// Store voxels which are inside voxel-space boundary.
	if (all(pin.svoPos>= 0) && all(pin.svoPos < gDim)) 
	{
		// map normal from -1~1 to 0~1 .
		float3 normal = (pin.normW + 1.f)*0.5f;	

		gTargetUAV[pin.svoPos] = float4(normal, 1.0f);
		//to make it easier to check the result.
		return float4(normal,0.0);
	}

	else return float4(0,0,0, 0);
}

technique11 VoxelizerTech
{
	pass VoxelizerPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetDepthStencilState(DisableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(RS_CullDisabled);
	}
}
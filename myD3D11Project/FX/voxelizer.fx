
#define PI 3.1415926

//-----------------------
//constant buffer
//---------------------

//todo register
cbuffer cbPerFrame 
{
	float4x4 gView;
	float4x4 gProj;
	int gRes;
	float3 gVoxelSize;
};

cbuffer cbPerObject 
{
	float4x4 gWorld;
	float gObjectID;
};

//--------------------------
//read write 3d texture
//---------------------------
//RWTexture3D<float4> gTargetUAV;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
};

struct VS_OUT
{
	float4 posW  : TEXCOORD0;
	float4 posV  : SV_POSITION;
	float3 normW : TEXCOORD1;
};

struct PS_IN
{
	float4 pos  : SV_POSITION;
	float4 posV  : TEXCOORD0;
	float3 normW : TEXCOORD2;
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

	vout.normW=mul(float4(vin.normL,1), gWorld).xyz;
	vout.normW = normalize(vout.normW);

	return vout;
}

//-----------------------------
//GEOMETRY SHADER
//-----------------------------
[maxvertexcount(3)]
void GS(triangle VS_OUT gin[3],inout TriangleStream<PS_IN> triStream)
{

	float x=abs(dot(gin[0].normW,float3(1,0,0)));
	float y=abs(dot(gin[0].normW,float3(0,1,0)));
	float z=abs(dot(gin[0].normW,float3(0,0,1)));

	float3 p0 = gin[1].posV - gin[0].posV;
	float3 p1 = gin[2].posV - gin[0].posV;
	float3 p = abs(cross(p0, p1));

	float testz=abs(dot(gin[0].normW,float3(0,0,1)));

	float normProjAxis=max(x,max(y,z));

		for (int i = 0; i < 3; i++)
	{
		PS_IN output;

		if(p.z > p.x && p.z > p.y)
		{
			output.pos = float4(gin[i].posV.xy, 0, 1);
		}
		else if(p.x > p.y && p.x > p.z)
		{
			output.pos = float4(gin[i].posV.yz, 0, 1);
		}
		else 
		{
			output.pos = float4(gin[i].posV.xz, 0, 1);
		}

		output.posV = gin[i].posV;	
		output.normW = gin[i].normW;

		triStream.Append(output);
	}

}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(PS_IN pin) : SV_Target
{

	// Use view-space position to access an address of Texture3D.
	float x = pin.posV.x / gVoxelSize.x;
	float y = pin.posV.y / gVoxelSize.y;
	float z = pin.posV.z / gVoxelSize.z;
	x = x + gRes / 2;
	y = y + gRes / 2;

	// Store voxels which are inside voxel-space boundary.
	if (x >= 0 && x < gRes && y >= 0 && y < gRes && z >= 0 && z < gRes)
	{
		float3 normal = (pin.normW + 1.f)*0.5f;	// Transform normal data from -1~1 to 0~1 for DXGI_FORMAT_R8G8B8A8_UNORM format.

		// Compress 3 components to 2 components.
		half scale = 1.7777;
		half2 enc = normal.xy / (normal.z + 1);
		enc /= scale;
		enc = enc*0.5 + 0.5;
		normal.xyz = float3(enc.xy, 0);

		//gTargetUAV[uint3(x, y, z)] = float4(normal, gObjectID);

	}
	// A trivial return-value because I don't set any render targets.
	return float4(0, 0, 0, 1);
}

technique11 VoxelizerTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		//SetDepthStencilState(DisableDepth, 0);
		//SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		//SetRasterizerState(RS_CullDisabled);
	}
}
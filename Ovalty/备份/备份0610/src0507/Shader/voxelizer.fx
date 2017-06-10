#include "common_tools.fx"
#include "brdf_tools.fx"
#include "compute.fx"

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	float gDim;
	float gVoxelSize;
	float3 gVoxelOffset;
	float3 gEyePosW;
	float gTime;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
};

//--------------------------
//read write 3d texture
//---------------------------
RWTexture3D<float4> gUAVColor;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL   : POSITION;
	float3 normL  : NORMAL;
	uint index	: SV_VertexID;
};

struct VS_OUT
{
	float4 posW  : SV_POSITION;
	float3 normW : NORMW;
	uint ID	     : ID;
};

struct PS_IN
{
	float4 pos    : SV_POSITION;	//pos just for rasterization
	float3 normW  : NORMW;
	float3 svoPos : SVO;	//voxel-space boundary coordinates xyz>=0 && xyz<=gDim
	float4 posW	  : POSW;
	uint ID	      : ID;
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

	float3 posL=vin.posL;
	if(vin.index>=3264&&vin.index<3300)
	{
		posL.y+=0.00001f;
	}
		

	vout.posW=mul(float4(posL,1.0f),gWorld);
	vout.normW=mul(float4(vin.normL,1.0f),gWorldInverTrans).xyz;
	vout.ID=vin.index;
	return vout;
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

		float offsetX=gVoxelOffset.x;
		float offsetY=gVoxelOffset.y;
		float offsetZ=gVoxelOffset.z;

		// Projection the main face
		if (axis == facenormal.x)
		{
			output.pos.xyz = gin[i].posW.zyx;
			offsetX=gVoxelOffset.z;
			offsetZ=gVoxelOffset.x;
		}
		else if (axis == facenormal.y)
		{
			output.pos.xyz = gin[i].posW.xzy;
			offsetY=gVoxelOffset.z;
			offsetZ=gVoxelOffset.y;			
		}
		else
		{
			output.pos.xyz = gin[i].posW.xyz;
		}

		output.svoPos=world_to_svo(gin[i].posW.xyz,gVoxelSize,gVoxelOffset);

		//pos for rasterization
		output.pos.xyz+=float3(offsetX,offsetY,offsetZ);
		output.pos.xyz /= (float)gDim;
		output.pos.xyz /= (float)gVoxelSize;
		output.pos.zw = 1;

		output.normW = gin[i].normW;

		output.posW=gin[i].posW;
		output.ID=gin[i].ID;

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(PS_IN pin) : SV_Target
{
	float3 litColor=0.0f;
	float4 diffuse, spec;

	// Store voxels which are inside voxel-space boundary.
	if (all(pin.svoPos>= 0) && all(pin.svoPos <= gDim) )
	{	
		MaterialBRDF mat;
		setMatCornellBox(pin.ID,mat);

		PointLightBRDF light[LIGHT_NUM];
		setPointLight(light[0],light[1],gTime);

		float3 V=normalize(gEyePosW - pin.posW.xyz);
		float3 N=normalize(pin.normW);

		for(uint i=0;i<LIGHT_NUM;i++)
		{
			float3 lightVec=light[i].position-pin.posW;
			float3 L=normalize(light[i].position-pin.posW.xyz);
			float3 H=normalize(V+L);
			litColor += DirectLighting(N, H, lightVec, V, L, light[i], mat);
			
			float3 light_visualize=world_to_svo(light[i].position,gVoxelSize,gVoxelOffset);
			gUAVColor[light_visualize] = float4(0.0,1.0,0.0,0.0);
		}

		//tonemapping
		litColor = ACESToneMapping(litColor.xyz, 1.0f);

		gUAVColor[pin.svoPos] = float4(litColor, 1.0f);

		//to make it easier to check the result.
		return float4(0,0,0,1);
	}

	else return float4(1,1,1, 0);
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
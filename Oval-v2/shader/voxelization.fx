#include "common_tools.fx"
#include "BRDF_tools.fx"

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame
{
	Matrix cb_Matrix;
	Voxel  cb_Voxel;
	Common cb_Common;
};

//--------------------------
//read/write 3d texture
//---------------------------
RWTexture3D<float4> uav_color;

//------------------------
//VERTEX SHADER
//------------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL : NORMAL;
	float2 tex   : TEXCOORD;
};

struct VS_OUT
{
	float4 posW  : SV_POSITION;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

VS_OUT vs_main(VS_IN vin)
{
	VS_OUT vout;

	vout.posW = mul(float4(vin.posL, 1.0f), cb_Matrix.world);

	vout.normW = mul(float4(vin.normL, 1.0f), cb_Matrix.world).xyz;

	vout.tex = vin.tex;

	return vout;
}

//------------------------
//GEOMETRY SHADER
//------------------------
struct GS_IN
{
    float4 posW  : SV_POSITION;
	float3 normW : NORMAL;
    float2 tex   : TEXCOORD;
};

struct GS_OUT
{
	float4 pos   : SV_POSITION;
	float3 svopos: SVO;
	float4 posW  : POSW;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

[maxvertexcount(3)]
void gs_main(triangle GS_IN gin[3], inout TriangleStream<GS_OUT> triStream)
{
	float3 facenormal=abs(normalize(cross(gin[1].posW.xyz-gin[0].posW.xyz,gin[2].posW.xyz-gin[0].posW.xyz)));
	float axis=max(facenormal.x,max(facenormal.y,facenormal.z));

	for( uint i = 0; i < 3; i++)
	{
		GS_OUT output;

		float offsetX=cb_Voxel.offset.x;
		float offsetY=cb_Voxel.offset.y;
		float offsetZ=cb_Voxel.offset.z;

		// Projection the main face
		if (axis == facenormal.x)
		{
			output.pos.xyz = gin[i].posW.zyx;
			offsetX=cb_Voxel.offset.z;
			offsetZ=cb_Voxel.offset.x;
		}
		else if (axis == facenormal.y)
		{
			output.pos.xyz = gin[i].posW.xzy;
			offsetY=cb_Voxel.offset.z;
			offsetZ=cb_Voxel.offset.y;			
		}
		else
		{
			output.pos.xyz = gin[i].posW.xyz;
		}

		output.svopos=world_to_svo(gin[i].posW.xyz,cb_Voxel.size,cb_Voxel.offset);

		//pos for rasterization
		output.pos.xyz+=float3(offsetX,offsetY,offsetZ);
		output.pos.xyz /= (float)cb_Voxel.dimension;
		output.pos.xyz /= (float)cb_Voxel.size;
		output.pos.zw = 1;

		output.normW = gin[i].normW;

		output.posW = gin[i].posW;

		output.tex = gin[i].tex;

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

//------------------------
//PIXEL SHADER
//------------------------
struct PS_IN
{
	float4 pos   : SV_POSITION;
	float3 svopos: SVO;
	float4 posW  : POSW;
	float3 normW : NORMAL;
	float2 tex   : TEXCOORD;
};

float4 ps_main(PS_IN pin) : SV_TARGET
{
	float3 litColor=0.0f;
	float4 diffuse, spec;

	// Store voxels which are inside voxel-space boundary.
	if (all(pin.svopos>= 0) && all(pin.svopos <= cb_Voxel.dimension) )
	{	
		MaterialBRDF mat;
		setMatCornellBox(0,mat);

		PointLightBRDF light[LIGHT_NUM];
		float gTime = 0;
		setPointLight(light[0],light[1],cb_Common.time);

		float3 V=normalize(cb_Common.eyeposW - pin.posW.xyz);
		float3 N=normalize(pin.normW);

		for(uint i=0;i<LIGHT_NUM;i++)
		{
			float3 lightVec=light[i].position-pin.posW.xyz;
			float3 L=normalize(light[i].position-pin.posW.xyz);
			float3 H=normalize(V+L);
			litColor += DirectLighting(N, H, lightVec, V, L, light[i], mat).xyz;
			
			float3 light_visualize=world_to_svo(light[i].position,cb_Voxel.size,cb_Voxel.offset);
			uav_color[light_visualize] = float4(0.0,1.0,0.0,1.0);
		}

		//tonemapping
		litColor=ACESToneMapping(litColor.xyz,1.0f);

		uav_color[pin.svopos] = float4(litColor,1.0f);

		//to make it easier to check the result.
		return float4(litColor, 1.0f);
	}

	else
		return float4(1, 1, 1, 0);
}

technique11 VOXELIZATION_TECH
{
	pass VOXELIZATION_PASS
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetGeometryShader(CompileShader(gs_5_0, gs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));

		SetDepthStencilState(DisableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}
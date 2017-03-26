#include"tools.fx"
//--------------------------
// light structure
//---------------------------
struct PointLight
{
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float Pad;
};

//--------------------------
// material structure
//---------------------------
struct Material
{
	float4 Diffuse;
	float4 Specular;
};

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	int gDim;
	float3 gVoxelSize;

	PointLight gPointLight;
	float3 gEyePosW;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInverTrans;
	Material gMat;
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
};

struct VS_OUT
{
	float4 posW  : SV_POSITION;
	float3 normW : NORMW;
};

struct PS_IN
{
	float4 pos    : SV_POSITION;	//pos just for rasterization
	float3 normW  : NORMW;
	float3 svoPos : SVO;	//voxel-space boundary coordinates xyz>=0 && xyz<=gDim
	float4 posW	  : POSW;
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
	vout.posW*=10.0f;
	vout.normW=mul(float4(vin.normL,1.0f),gWorldInverTrans).xyz;

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
		
		//at first I try to map raster coordinate to [0,255] to pair cualquier model,but the "exact" map will loss accuracy
		//here we simply give the offset and keep it in mind
		float3 offset=0.5*gDim;
		uint x=ceil(output.pos.x+offset);
		uint y=ceil(output.pos.y+offset);
		uint z=ceil(output.pos.z+offset);

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
		output.pos.xyz /= (float)gDim;
		output.pos.zw = 1;

		output.normW = gin[i].normW;

		output.posW=gin[i].posW;

		triStream.Append(output);
	}
	triStream.RestartStrip();
}

//----------------------------
//compute point light color in pixel shader
//-------------------------
void ComputePointLight(float3 pos,float3 normal,float3 toEye,
						out float4 diffuse, out float4 spec)
{
	diffuse=float4 (0,0,0,0);
	spec=float4 (0,0,0,0);

	normal=normalize(normal);

	float3 lightVec=gPointLight.Position-pos;

	//distance from light to surface
	float d=length(lightVec);

	//if(d>gPointLight.Range) return;

	//normalize light vector
	lightVec=normalize(lightVec);

	//diffuse lighting part
	float diffFactor=dot(lightVec,normal);
	if(diffFactor>0.0f)
	{

	diffuse=diffFactor *gPointLight.Diffuse*gMat.Diffuse;

	//spec lighting part
	float3 r=reflect(-lightVec,normal);
	float specFactor=pow(max(dot(toEye,r),0.0f),gMat.Specular.a);
	spec = specFactor * gMat.Specular * gPointLight.Specular;
	}
	
	float attFactor= 1.0f / dot(gPointLight.Attenuation, float3(1.0f, d, d*d));;
	diffuse *= attFactor;
	spec    *= attFactor;
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(PS_IN pin) : SV_Target
{
	float4 litColor,diffuse, spec;

	// Store voxels which are inside voxel-space boundary.
	if (all(pin.svoPos>= 0) && all(pin.svoPos < gDim)) 
	{	
		float3 toEyeW = normalize(gEyePosW - pin.posW.xyz);

		ComputePointLight(pin.posW.xyz, pin.normW, toEyeW,
									diffuse, spec);
		litColor = diffuse +spec;
		litColor.a= gMat.Diffuse.a;
		 
		gUAVColor[pin.svoPos] = litColor;

		uint3 light_visualize=world_to_svo(gPointLight.Position,gDim);
		gUAVColor[light_visualize] = float4(1.0,0.0,0.0,1.0);

		//to make it easier to check the result.
		return float4(litColor);
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
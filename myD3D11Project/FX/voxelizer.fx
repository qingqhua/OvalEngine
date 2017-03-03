
#define PI 3.1415926

//-----------------------
//constant buffer
//---------------------

//todo register
cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
	int gRes;
	float3 gVoxelSize;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float gObjectID;
};

//--------------------------
//read write 3d texture
//---------------------------
RWTexture3D<float4> gTargetUAV	: register(u1);

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
};

struct VS_OUT
{
	float4 posW  : TEXCOORD0;
	float4 posV  : SV_POSITION;
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

	return vout;
}

//-----------------------------
//GEOMETRY SHADER
//-----------------------------
[maxvertexcount(3)]
void GS(triangle VS_OUT gin[3],inout TriangleStream<PS_IN> triStream)
{
	// Initial data.
	float3 newPos[3];
	float3 centerPos = float3(0, 0, 0);
	float3 normal = float3(0, 0, 0);

	float4x4 gRot = { 1.f, 0.f, 0.f, 0.f,0.f, 1.f, 0.f, 0.f,0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f };

	// Find the center of triangle.
	for (int j = 0; j < 3; j++)
	{
		centerPos += gin[j].posW.xyz;
	}
	centerPos /= 3;
	//compute normal
	float3 p0 = gin[1].posW.xyz - gin[0].posW.xyz;
	float3 p1 = gin[2].posW.xyz - gin[0].posW.xyz;
	normal = cross(p0,p1);

	normal=normalize(normal);

	float eye_angle = dot(normal, float3(0, 0, 1));
	float3 v = cross(normal, float3(0, 0, 1));
	v = normalize(v);

	// Make sure v is not float3(0,0,0),
	if (dot(v, v) > 0) {

		// Create a rotation matrix to rotate the triangle to the opposite with view direction.
		float cost = eye_angle, sint = pow(1 - eye_angle*eye_angle, 0.5f), one_sub_cost = 1 - cost;
		float4x4 RotToCam = { v.x * v.x * one_sub_cost + cost, v.x * v.y * one_sub_cost + v.z * sint, v.x * v.z * one_sub_cost - v.y * sint, 0, \
			v.x * v.y * one_sub_cost - v.z * sint, v.y * v.y * one_sub_cost + cost, v.y * v.z * one_sub_cost + v.x * sint, 0, \
			v.x * v.z * one_sub_cost + v.y * sint, v.y * v.z * one_sub_cost - v.x * sint, v.z * v.z * one_sub_cost + cost, 0, \
			0, 0, 0, 1 };
		gRot = RotToCam;
	}

	// Apply rotation.
	for (int k = 0; k < 3; k++)
	{
		newPos[k] = gin[k].posW.xyz - centerPos;
		newPos[k] = mul(float4(newPos[k], 1.0f), gRot).xyz;
	}

	// Build a bounding box of this triangle in order to control the density of pixels that a triangle can produce.
	float minX = min(min(newPos[0].x, newPos[1].x), newPos[2].x);
	float maxX = max(max(newPos[0].x, newPos[1].x), newPos[2].x);
	float minY = min(min(newPos[0].y, newPos[1].y), newPos[2].y);
	float maxY = max(max(newPos[0].y, newPos[1].y), newPos[2].y);

	float2 RasterSize = (2 / float2(maxX - minX, maxY - minY));

	// Apply orthogonal projection.
	for (int i = 0; i < 3; i++)
	{
		PS_IN output;
		// Transform x,y to [-1,1].
		newPos[i].xy = (newPos[i].xy - float2(minX, minY))  * RasterSize.xy - 1;
		output.pos = float4(newPos[i].xy, 1, 1);
		output.posV = gin[i].posV;	// Assign view-space voxel positions.
		output.normW = normal;

		triStream.Append(output);
	}
	triStream.RestartStrip();

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
	x = x + gRes / 2.0f;
	y = y + gRes / 2.0f;

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

		gTargetUAV[uint3(x, y, z)] = float4(normal, gObjectID);

	}
	// A trivial return-value because I don't set any render targets.
	return float4(0, 0, 0, 0);
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
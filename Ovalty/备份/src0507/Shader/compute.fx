//--------------------------
// compute shader to access data
//---------------------------

cbuffer compute_buffer
{
	float4x4 c_World;
	float4x4 c_WorldInverTrans;
	float4x4 c_View;
	float4x4 c_Proj;

	float3 c_EyePosW;

	float c_Dim;
	float c_VoxelSize;
	float3 c_VoxelOffset;
	
	float c_Time;
};

[numthreads(16,16,1)]
void CS(int3 dispatchThreadID : SV_DispatchThreadID)
{

}

technique11 ComputeTech
{
	pass P0
	{
		SetVertexShader(NULL);
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetComputeShader(CompileShader(cs_5_0, CS()));
	}
}
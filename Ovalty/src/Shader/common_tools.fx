//-----------------------------------------------------------------------------------------
// common tools for  corninates transform, fliter, etc.
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Default Sampler.
//-----------------------------------------------------------------------------------------
SamplerState SVOFilter
{
	//Filter = ANISOTROPIC;
	//MaxAnisotropy = 0;
	//MipMapLevelOfDetailBias=0;
};

//-----------------------------------------------------------------------------------------
// give offset to transfer to voxel coordinate
//-----------------------------------------------------------------------------------------
float3 world_to_svo(float3 posW,float voxel_size,float3 offset)
{
	float3 pos=posW;
	pos=((pos+offset)/voxel_size);
	return pos;
}

float3 svo_to_world(float3 posW,float voxel_size,float3 offset)
{
	float3 pos=posW;
	pos*=voxel_size;
	pos-=offset;
	return pos;
}

//-----------------------------------------------------------------------------------------
// map from -1,1 to 0,1
//-----------------------------------------------------------------------------------------
float map(float from)
{
	return float(0.5f*from+0.5f);
}

float3 orthogonal(float3 u){
	float3 v = float3(0.577, 0.577, 0.577); 
	return abs(dot(u, v)) > 0.99999f ? cross(u, float3(1, 0, 0)) : cross(u, v);
}
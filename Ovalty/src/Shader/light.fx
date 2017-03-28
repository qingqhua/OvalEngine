//--------------------------
// direct light structure
//---------------------------
struct PointLightBRDF
{
	float3 position;
	float3 color;
};

//--------------------------
// material structure
//---------------------------
struct MaterialBRDF
{
	float3 diffAlbedo;
	float metallic;
	float3 specAlbedo;
	float roughness;
};

//--------------------------
// set material attribute
//---------------------------
void setMatGlossy(out MaterialBRDF M)
{
	M.diffAlbedo = float3(0.6f, 0.6f, 0.6f);
	M.specAlbedo = 0.6f;
	M.metallic = 0.1f;
	M.roughness = 0.4f;
}

void setMat(out MaterialBRDF M)
{
	M.diffAlbedo = float3(1.0f, 0.5f, 0.2f);
	M.specAlbedo = float3(1.5f, 0.8f, 0.0f);
	M.metallic = 0.0f;
	M.roughness = 0.1f;
}

void setMatPerObject(int id,out MaterialBRDF mat)
{
	//sphere
	if(id<3264)
	 setMatGlossy(mat);
	//short box
	else if(id>=3264&&id<3300)
		  setMat(mat);
	//floor
	else if(id>=3300&&id<3306)
		 setMatGlossy(mat);
	//ceiling
	else if(id>=3306&&id<3312)
		 setMatGlossy(mat);
	//backwall
	else if(id>=3312&&id<3318)
		 setMatGlossy(mat);
	//rightwall
	else if(id>=3324&&id<3330)
		 setMatGlossy(mat);
	//leftwall
	else if(id>=3300&&id<3336)
		 setMatGlossy(mat);
}

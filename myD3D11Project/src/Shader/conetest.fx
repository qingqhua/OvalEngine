//--------------------------
// direct light structure
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

	//float emissivity;
	//float transparency;
};

//-----------------------
//constant buffer
//---------------------
cbuffer cbPerFrame : register(b0)
{
	float3 gEyePosW;
	PointLight gPointLight;
	float4x4 gView;
	float4x4 gProj;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	Material gMat;
};


//--------------------------
//read voxel from 3d texture
//---------------------------
Texture3D<float4> gVoxelList;

//--------------------------
//set sampler
//---------------------------
SamplerState SVOFilter;

//----------------------
//shader structure
//--------------------
struct VS_IN
{
	float3 posL  : POSITION;
	float3 normL  :NORMAL;
};

struct VS_OUT
{
	float4 posW   : POSITION;
	float4 posH   : SV_POSITION;
	float3 normW  : NORMAL;
};

struct Ray {
	float3 origin; // Origin
	float3 direction; // Direction
	float t; //distance
};

//-----------------------------
//VERTEX SHADER
//-----------------------------
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	vout.posW=mul(float4(vin.posL,1.0f),gWorld);
	vout.posH=mul(vout.posW,gView);
	vout.posH=mul(vout.posH,gProj);

	vout.normW=mul(float4(vin.normL,1.0f),gWorld).xyz;
	return vout;
}


// Traces a diffuse voxel cone.
float3 traceDiffuseVoxelCone(const float3 from, float3 direction){
	direction = normalize(direction);
	
	const float CONE_SPREAD = 0.325;

	float4 acc = float4(0.0f,0.0f,0.0f,0.0f);

	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.1953125;

	// Trace.
	while(dist < pow(2,0.5) && acc.a < 1){
		float3 c = from + dist * direction;
		//c = scaleAndBias(from + dist * direction);
		float l = (1 + 0.325 * dist / 1.0f);
		float level = log2(l);
		float ll = (level + 1) * (level + 1);
		
		//float4 voxel = textureLod(texture3D, c, min(5.4f, level));
		float4 voxel=gVoxelList[0,0,0];
		acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
		dist += ll * 1.0f * 2;
	}
	return pow(acc.rgb * 2.0, float3(1.5));
}

//----------------------------
//PIXEL SHADER
//-------------------------
float4 PS(VS_OUT pin) : SV_Target
{
	float3 viewVec=normalize(pin.posW.xyz-gEyePosW);

	float4 rgba=float4(0.0,1.0,0.0,1.0f);
	return rgba;
}

RasterizerState SolidRS
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

// Sphere represents of centre and radius of a sphere
struct Sphere {
	float3 center;  // Center
	float radius; // Radius
};

float4 diffuse(in float3 surface, in float3 center, in float4 color, in float3 litePos) {
	// Surface normal
	float3 n = normalize(surface - center);
	
	// Light direction from surface
	float3 l = normalize(litePos - surface);

	// The diffuse equation
	return color * max(0.0, dot(n, l));
}

float intersectSphere(in Ray ray, in Sphere sphere) {
	// Sphere center to ray origin
	float3 co = ray.origin - sphere.center;

	// The discriminant is negative for a miss, or a postive value
	// used to calcluate the distance
	float discriminant = dot(co, ray.direction) * dot(co, ray.direction)
			- (dot(co, co) - sphere.radius * sphere.radius);

	// If answer is not negative, calculate the origin-surface distance
	if (discriminant >= 0.0)
		return -dot(ray.direction, co) - sqrt(discriminant); //
	else
		return -1.; // Any negative number to indicate no intersect
}

float4 PS_TEST(VS_OUT pin): SV_Target
 {
	float4 color=float4(0,0,0,0);
	Sphere gsphere;
	Ray gray;

	float3 pixelPos = float3(pin.posH.xyz);

    // The eye position in this example is fixed.
    float3 eyePos = gEyePosW; // Some distance in front of the screen
    
	// The ray for the raytrace - which is just intersectSphere in this tutorial
	float3 rayDir = normalize(pixelPos - eyePos);
    
    gsphere.center = float3(0.0, 0.0, 0.0); // Position and size of sphere
	gsphere.radius=10.0f;

	// Does rayDirUnit ray from ndcEye intersect the sphere?
	gray.origin=eyePos;
	gray.direction=rayDir;
	float eyeToSphere = intersectSphere(gray, gsphere);

	// If positive, then we draw a pixel of the sphere
	if (eyeToSphere >= 0.)
	{
		// Choose a diffuse colour
		float4 diffuseColour =  float4(0.,1.,1.,1);

		// Choose an ambient colour
		float4 ambientColour =  float4(0.2,0.,0.,1);

        // Position of a white light
        float3 litePos = float3(3., 0., 0.);
        
		color = ambientColour + diffuse(eyePos + eyeToSphere * rayDir, gsphere.center, diffuseColour, litePos);
		
	} else
        // Otherwise we draw the colour of the background
		color = float4(0, 0, 0, 1);

		return color;
}
technique11 ConeTracingTech
{
	pass ConeTracingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_TEST()));
		SetRasterizerState(SolidRS);
	}
}

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



#define MAX_ITER	100
#define MAX_DIST	10.0
#define EPSILON		0.001


float torus(float3 pos, float2 t)
{
    float2 q = float2(length(0.80*pos.xz + 1.75) - 2.75*t.x, 1.25*pos.y);
    return 1.1*length(q) - t.y;
}

float distFunc(float3 pos)
{
    float2 t = float2(0.88,0.88);
    return torus(pos, t);
}

float4 mainImage(VS_OUT pin):SV_TARGET
{
	float4 fragColor=float4(0,0,0,0);

	float3 camOrigin	= float3(2.0,2.0,2.0);
    float3 camTarget	= float3(0.0,0.0,0.0);
    float3 upDir		= float3(0.0, 1.0, 0.0);
    
    float3 camDir		= normalize(camTarget - camOrigin);
    float3 camRight	= normalize(cross(upDir, camOrigin));
    float3 camUp		= cross(camDir, camRight);
    
    //place origin at center of screen & correct aspect ratio
    float2 screenPos = -1.0 + 2.0 * pin.posW.xy / float2(800,600);
    screenPos.x *= 800 / 600;
    
    float3 rayDir = normalize(camRight * screenPos.x + camUp * screenPos.y + camDir);
    
    //Raymarching loop
    float totalDist = 0.0;
    float3 pos = camOrigin;
    float dist = EPSILON;
    
    for (int i = 0; i < MAX_ITER; i++) {
        if (dist < EPSILON || totalDist > MAX_DIST) {
            break;
        }
        
        dist = distFunc(pos);
        totalDist += dist;
        pos += dist * rayDir;
        
    }
    if (dist < EPSILON) {				//the ray hit the object
    	float2 eps = float2(0.0, EPSILON);
    	float3 normal = normalize(float3(
       			distFunc(pos + eps.yxx) - distFunc(pos - eps.yxx),
       			distFunc(pos + eps.xyx) - distFunc(pos - eps.xyx),
       			distFunc(pos + eps.xxy) - distFunc(pos - eps.xxy)));
    
    	float diffuse = max(0.0, dot(-rayDir, normal));
    	float specular = pow(diffuse, 128.0);
        float lighting = diffuse + specular;
        float toonMap;
        
        //map lighting information to discrete values
        if (lighting < 0.256) {
            toonMap = 0.195;
        } else if (lighting < 0.781) {
            toonMap = 0.781;
        } else {
            toonMap = 0.900;
        }
        float3 color = float3(toonMap,toonMap,toonMap);
        fragColor = float4(color, 1.0);
    } else {						//the ray didn't hit anything
        fragColor = float4(0.5, 0.7, 0.7, 1.0);
    }
	return fragColor;
}

technique11 ConeTracingTech
{
	pass ConeTracingPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, mainImage()));
		SetRasterizerState(SolidRS);
	}
}
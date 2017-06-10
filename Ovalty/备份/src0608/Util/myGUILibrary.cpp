#include "myGUILibrary.h"

using namespace DirectX;

myGUILibrary::myGUILibrary()
{
	Reset();
}

myGUILibrary::~myGUILibrary()
{

}

void TW_CALL ReinitCB(void *clientData)
{
	myGUILibrary *scene = static_cast<myGUILibrary *>(clientData); // scene pointer is stored in clientData
	scene->Reset();                               // re-initialize the scene
}

void myGUILibrary::Init(HWND _hwnd, ID3D11Device* _d3dDevice, float _screenwidth, float _screenheight,myShapeLibrary* _shape)
{
	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11, _d3dDevice))
		MessageBoxA(_hwnd, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);

	TwWindowSize((INT)_screenwidth, (INT)_screenheight);

	// Create a tweak bar
	TwBar *bar = TwNewBar("OvalEngine");
	int barSize[2] = { _screenwidth / 4, _screenheight / 2 };
	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	TwDefine(" OvalEngine alpha=128 valueswidth=0");
	TwDefine(" GLOBAL fontSize=3 help='Oval Engine: an experimental Rendering Engine based on DirectX11.'");
	
	//add rendering mode
	TwEnumVal modeEV[] = {{ VOXELIZE, "Voxelize" },
						{ LOCAL, "Local Lighting" },
						{ GI, "Global Illumination"},
						{ CARTOON, "Cartoon Shading"},
						{ WIREFRAME, "WireFrame"} };

	TwType modeType = TwDefineEnum("Rendering Mode", modeEV, 5);
	TwAddVarRW(bar, "Rendering Mode", modeType, &renderMode, "");

	//add material
	TwAddVarRW(bar, "Albedo", TW_TYPE_COLOR3F, &matAlbedo, "opened=true group=MATERIAL colormode=rgb");
	TwAddVarRW(bar, "Roughness", TW_TYPE_FLOAT, &matRough, "group=MATERIAL min=0 max=1 step=0.1");
	TwAddVarRW(bar, "Metallic", TW_TYPE_FLOAT, &matMetal, "group=MATERIAL min=0 max=1 step=0.1");

	//add light
	TwAddVarRW(bar, "albedo", TW_TYPE_COLOR3F, &lightAlbedo, "opened=true group=LIGHT colormode=rgb");
	TwAddVarRW(bar, "POSITION", TW_TYPE_QUAT4F, &lightPos, "opened=true group=LIGHT axisz=-z min=-1 max=1 step=0.1");
	TwAddVarRW(bar, "POS X", TW_TYPE_FLOAT, &lightPos.x, "group=LIGHT min=-1 max=1 step=0.1");
	TwAddVarRW(bar, "POS Y", TW_TYPE_FLOAT, &lightPos.y, "group=LIGHT min=0 max=2 step=0.1");
	TwAddVarRW(bar, "POS Z", TW_TYPE_FLOAT, &lightPos.z, "group=LIGHT min=-2 max=1 step=0.1");
	TwAddVarRW(bar, "Radius", TW_TYPE_FLOAT, &lightRadius, "group=LIGHT min=0 max=1 step=0.1");

	//back color
	TwAddVarRW(bar, "BACKGROUND", TW_TYPE_COLOR3F, &backColor, "colormode=rgb");

	//reset
	TwAddButton(bar, "Reset", ReinitCB, this,"");
}

void myGUILibrary::Resize(float _screenwidth, float _screenheight)
{
	//rest main bar size
	TwBar *bar = TwGetBarByName("OvalEngine");
	int barSize[2] = { _screenwidth / 4, _screenheight / 2 };
	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	//reset help bar size
	TwBar *sbar = TwGetBarByName("TW_HELP");
	TwDefine("TW_HELP position='10 600' size='400 200' ");
}

void myGUILibrary::Reset()
{
	//init light
	lightPos = XMFLOAT4(1000.4f, 1.2f, -0.5f, 1.0f);
	lightRadius = 0.5;
	lightAlbedo = XMFLOAT3(1.0f, 1.0f, 1.0f);

	//init mat
	matAlbedo = XMFLOAT3(1.0f, 241.0f / 256.0f, 0.0);
	matRough = 1.0f;
	matMetal = 0.0f;

	backColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	renderMode = GI;
}


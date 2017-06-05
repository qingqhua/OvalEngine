#include "myGUILibrary.h"

using namespace DirectX;

myGUILibrary::myGUILibrary()
{




}

myGUILibrary::~myGUILibrary()
{

}

void myGUILibrary::Init(HWND _hwnd, ID3D11Device* _d3dDevice,float _screenwidth,float _screenheight, 
						DirectX::XMFLOAT4 *backcolor,int *MODE,
						DirectX::XMFLOAT4 *lightpos, DirectX::XMFLOAT3 *lightAldebo, 
						DirectX::XMFLOAT3 *matAldebo,float *matRough,float *matMetal)
{
	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11, _d3dDevice))
		MessageBoxA(_hwnd, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);

	TwWindowSize((INT)_screenwidth, (INT)_screenheight);

	// Create a tweak bar
	TwBar *bar = TwNewBar("OvalBar");
	//int barSize[2] = { 200, 3 };
	//TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	TwAddVarRW(bar, "MODE", TW_TYPE_UINT8, MODE, "min=0 max=3 step=1");

	//light pos
	TwAddVarRW(bar, "LIGHT POSITION", TW_TYPE_QUAT4F, lightpos, "min=-1 max=1 step=0.1");
	TwAddVarRW(bar, "LIGHT POS X", TW_TYPE_FLOAT, &lightpos->x, "min=-1 max=1 step=0.1");
	TwAddVarRW(bar, "LIGHT POS Y", TW_TYPE_FLOAT, &lightpos->y, "min=0 max=2 step=0.1");
	TwAddVarRW(bar, "LIGHT POS Z", TW_TYPE_FLOAT, &lightpos->z, "min=-2 max=1 step=0.1");

	//light aldebo
	TwAddVarRW(bar, "LIGHT ALDEBO", TW_TYPE_COLOR3F, lightAldebo, "colormode=rgb");

	//material aldebo
	TwAddVarRW(bar, "MAT ALDEBO", TW_TYPE_COLOR3F, matAldebo, "colormode=rgb");
	TwAddVarRW(bar, "MAT ROUGH", TW_TYPE_FLOAT, matRough, "min=0 max=1 step=0.1");
	TwAddVarRW(bar, "MAT METAL", TW_TYPE_FLOAT, matMetal, "min=0 max=1 step=0.1");

	//back color
	TwAddVarRW(bar, "BACKGROUND", TW_TYPE_COLOR4F, backcolor, "colormode=rgb");
	//TwAddVarCB(bar, "Level", TW_TYPE_INT32, SetSpongeLevelCB, GetSpongeLevelCB, NULL, "min=0 max=3 group=Sponge keyincr=l keydecr=L");

	//TwAddVarRW(bar, "voxel level", TW_TYPE_FLOAT, res, " min=32 max=256 step=32");
}


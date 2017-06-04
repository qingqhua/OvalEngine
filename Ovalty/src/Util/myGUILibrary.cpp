#include "myGUILibrary.h"

using namespace DirectX;

myGUILibrary::myGUILibrary()
{




}

myGUILibrary::~myGUILibrary()
{

}

void myGUILibrary::Init(HWND _hwnd, ID3D11Device* _d3dDevice,float _screenwidth,float _screenheight,XMFLOAT4 *backcolor,int *MODE)
{
	// Initialize AntTweakBar
	if (!TwInit(TW_DIRECT3D11, _d3dDevice))
		MessageBoxA(_hwnd, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);

	TwWindowSize(_screenwidth, _screenheight);

	// Create a tweak bar
	TwBar *bar = TwNewBar("OvalBar");
	//int barSize[2] = { 200, 3 };
	//TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	TwAddVarRW(bar, "MODE", TW_TYPE_UINT8, MODE, "min=0 max=3 step=1");
	TwAddVarRW(bar, "Background", TW_TYPE_COLOR4F, backcolor, "colormode=rgb");
	//TwAddVarCB(bar, "Level", TW_TYPE_INT32, SetSpongeLevelCB, GetSpongeLevelCB, NULL, "min=0 max=3 group=Sponge keyincr=l keydecr=L");

	//TwAddVarRW(bar, "voxel level", TW_TYPE_FLOAT, res, " min=32 max=256 step=32");
}


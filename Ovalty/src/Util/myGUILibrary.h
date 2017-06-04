#ifndef GUI_LIB
#define GUI_LIB

#include "3rdParty/AntTweakBar.h"
#include "Core/d3dUtil.h"

class myGUILibrary
{
public:
	myGUILibrary();
	~myGUILibrary();

	void Init(HWND _hwnd, ID3D11Device* _d3dDevice, float _screenwidth, float _screenheight, DirectX::XMFLOAT4 *backcolor, int *MODE);
private:

};

#endif // GUI_LIB
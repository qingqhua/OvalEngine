#ifndef GUI_LIB
#define GUI_LIB

#include "3rdParty/AntTweakBar.h"
#include "Core/d3dUtil.h"

class myGUILibrary
{
public:
	enum    RenderMode { VOXELIZE = 0, LOCAL, GI, CARTOON,WIREFRAME };

	RenderMode renderMode;

	//light
	DirectX::XMFLOAT3 lightAlbedo;
	DirectX::XMFLOAT4 lightPos;
	float lightRadius;

	//material
	DirectX::XMFLOAT3 matAlbedo;
	float matRough;
	float matMetal;

	DirectX::XMFLOAT3 backColor;

	myGUILibrary();
	~myGUILibrary();

	void Init(HWND _hwnd, ID3D11Device* _d3dDevice, float _screenwidth, float _screenheight, myShapeLibrary* _shape);
	void Resize(float _screenwidth, float _screenheight);
	void Reset();
private:

};

#endif // GUI_LIB
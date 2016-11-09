#pragma once

#include "d3dApp.h"
#include "Camera.h"


class myDirectX11 : public D3DApp
{
public:
	myDirectX11(HINSTANCE hInstance);
	~myDirectX11();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	//Init
	void BuildGeometryBuffer();
	void BuildFX();
	void BuildVertexLayout();
	void BuildTexture();

	void SetMat();
	void SetLight();

	//effect
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;

	//buffer
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	ID3D11InputLayout* mInputLayout;

	//matrix
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectMatrixVariable* mfxView;

	//light
	ID3DX11EffectVariable* mfxLight;
	ID3DX11EffectVariable* mfxEyePos;

	//material
	ID3DX11EffectVariable* mfxMatBox;
	ID3DX11EffectVariable* mfxMatBackWall;
	ID3DX11EffectVariable* mfxMatFloor;

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 m2ndWorld;
	DirectX::XMFLOAT3 mEyePos;

	Camera camera;

	POINT mLastMousePos;
};
#pragma once

#include "d3dApp.h"

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
	//matrix init
	void InitMatrix();

	void InitLight();

	//create&set vertex&index buffer
	void BuildGeometryBuffer();

	//compile FX
	void BuildFX();

	void TryGeometryBuffers();

	//create input layout 
	void BuildVertexLayout();

	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxProj;
	ID3DX11EffectMatrixVariable* mfxView;

	ID3DX11EffectVariable* mfxLight;

	ID3D11InputLayout* mInputLayout;

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 m2ndWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
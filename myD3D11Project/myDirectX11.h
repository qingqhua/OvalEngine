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
	//create&set vertex&index buffer
	void BuildGeometryBuffer();

	//compile FX
	void BuildFX();

	//create input layout 
	void BuildVertexLayout();

	void BuildTexture();

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
	ID3DX11EffectVariable* mfxMat;
	ID3DX11EffectVariable* mfxEyePos;

	//texture
	ID3DX11EffectShaderResourceVariable* mfxTextureSRV;
	ID3D11ShaderResourceView* mDiffuseMapSRV;

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 m2ndWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
	DirectX::XMFLOAT3 mEyePos;
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
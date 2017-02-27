//-----------------------------------
//FILE:myDirectX11.h
//main file
//-----------------------------------

#pragma once

#include "d3dApp.h"
#include "Voxel/Voxelizer.h"


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

	void BuildGeometryBuffer();

private:

	//buffer
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	//light
	ID3DX11EffectVariable* mfxLight;
	ID3DX11EffectVariable* mfxMat;
	ID3DX11EffectVariable* mfxEyePos;

	//texture
	ID3DX11EffectShaderResourceVariable* mfxTextureSRV;
	ID3D11ShaderResourceView* mDiffuseMapSRV;

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
	DirectX::XMFLOAT3 mEyePos;
	float mTheta;
	float mPhi;
	float mRadius;

	int indexCount;

	POINT mLastMousePos;

	//voxel attribute
	Voxelizer mVoxelizer;
	float mVoxelSize;
	bool mVoxelFlag;
	DirectX::BoundingBox mBoundingBox;
};
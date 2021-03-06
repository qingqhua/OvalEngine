//-----------------------------------
//FILE:myDirectX11.h
//main file
//-----------------------------------

#pragma once

#include "Core/d3dApp.h"
#include "Util/Camera.h"

#include "Voxelization/Voxelizer.h"
#include "Voxelization/Visualizer.h"
#include "ConeTracing/ConeTracer.h"

class myDirectX11 : public D3DApp
{
public:
	myDirectX11(HINSTANCE hInstance);
	~myDirectX11();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void resetOMTargetsAndViewport();
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	
private:
	void ControlCamera(float dt, float speed);

	void Initvoxel(float res);
	void Clear();
private:

	//buffer
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	//light
	ID3DX11EffectVariable* mfxLight;
	ID3DX11EffectVariable* mfxMat;

	//texture
	ID3DX11EffectShaderResourceVariable* mfxTextureSRV;
	ID3D11ShaderResourceView* mDiffuseMapSRV;

	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mWorldInversTrans;

	POINT mLastMousePos;

	//voxel attribute
	float mVoxelSize;
	bool mVoxelFlag;

	//main object to load shader
	Voxelizer mVoxelizer;
	Visualizer mVisualizer;
	ConeTracer mConeTracer;

	//camera
	Camera mCam;

	//model
	myShapeLibrary m_model_box;
	myShapeLibrary m_model_bunny;

};
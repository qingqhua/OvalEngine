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
#include "Util/myShapeLibrary.h"
#include "Util/myGUILibrary.h"

class myDirectX11 : public D3DApp
{
public:
	myDirectX11(HINSTANCE hInstance);
	~myDirectX11();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	void resetOMTargetsAndViewport();

	//control
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	
private:
	void ControlCamera(float dt, float speed);

	void Initvoxel(float res);

	void InitGUI();

	DirectX::XMFLOAT3 GetVoxelOffset(DirectX::BoundingBox AABB);
	float GetVoxelSize(DirectX::BoundingBox AABB, float res);

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
	float mRes;
	int MODE;
	bool OVER;

	//tool bar attribute
	DirectX::XMFLOAT4 mbackColor;

	//util object
	Voxelizer mVoxelizer;
	Visualizer mVisualizer;
	ConeTracer mConeTracer;

	Camera mCam;

	myShapeLibrary mshape_box;
	myShapeLibrary mshape_bunny;
	myGUILibrary mGUI;
};
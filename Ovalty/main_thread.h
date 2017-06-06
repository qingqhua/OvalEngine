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

	void updateLightMat();

	DirectX::XMFLOAT3 GetVoxelOffset(DirectX::BoundingBox AABB);
	float GetVoxelSize(DirectX::BoundingBox AABB, float res);

	void Clear();

private:
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mWorldInversTrans;

	POINT mLastMousePos;

	//voxel attribute
	float mRes;

	//util object 
	Voxelizer mVoxelizer;
	Visualizer mVisualizer;
	ConeTracer mConeTracer;

	Camera mCam;

	//light
	MyLightLibrary::PointLightBRDF mLight;

	//material
	MyLightLibrary::MaterialBRDF mMat;

	myShapeLibrary mshape;
	myGUILibrary mGUI;

	int kick;
};
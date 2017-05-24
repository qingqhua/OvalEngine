//-----------------------------------
//FILE:myDirectX11.cpp
//-----------------------------------

#include "main_thread.h"

using namespace DirectX;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	myDirectX11 theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

myDirectX11::myDirectX11(HINSTANCE hInstance)
	: D3DApp(hInstance),
	mBoxVB(0), mBoxIB(0),
	mfxLight(0), mfxMat(0),
	mfxTextureSRV(0), mDiffuseMapSRV(0)
{
	mMainWndCaption = L"Oval-Engine";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}

myDirectX11::~myDirectX11()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
}

bool myDirectX11::Init()
{
	if (!D3DApp::Init())
		return false;

	//init world matrix
	mWorld = XMMatrixIdentity();
	//mWorld = XMMatrixMultiply(XMMatrixScaling(0.1, 0.1,0.1),XMMatrixIdentity());
	mWorldInversTrans = myMathLibrary::InverseTranspose(mWorld);

	//model init
	mshape_box.Init(md3dDevice, "data/Model/bunny.obj", "data/Model/");

	mRes = 128.0f;
	Initvoxel(mRes);

	return true;
}

void myDirectX11::OnResize()
{
	D3DApp::OnResize(); 

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	mCam.SetLens(0.25f*myMathLibrary::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void myDirectX11::UpdateScene(float dt)
{
	ControlCamera(dt,2.0f);
	mCam.UpdateViewMatrix();
}


void myDirectX11::DrawScene()
{
	//clear buffer to begin scene
	Clear();

	//render model
	mshape_box.Render(md3dImmediateContext);
	//-----------------------
	//voxelize
	//---------------------
	//mVoxelizer.ResetViewPort();
	mVoxelizer.SetMatrix(&mWorld, &mWorldInversTrans, &mCam.View(), &mVoxelizer.GetProjMatrix(), mCam.GetPosition());
	mVoxelizer.Render(mTimer.TotalTime(), mshape_box.GetIndexCount(), GetVoxelSize(mshape_box.GetAABB(), mRes), GetVoxelOffset(mshape_box.GetAABB()));
	resetOMTargetsAndViewport();


	//-----------------------
	//render visualizer
	//---------------------
	mVisualizer.SetMatrix(&mWorld, &mWorldInversTrans, &mCam.View(), &mCam.Proj());
	mVisualizer.Render(mVoxelizer.GetSRV(), GetVoxelSize(mshape_box.GetAABB(), mRes), GetVoxelOffset(mshape_box.GetAABB()));

	//-----------------------
	//render cone tracing
	//---------------------
	mConeTracer.SetMatrix(&mWorld, &mWorldInversTrans, &mCam.View(), &mCam.Proj(), mCam.GetPosition());
	mConeTracer.Render(mVoxelizer.GetSRV(), mTimer.TotalTime(), mshape_box.GetIndexCount(), GetVoxelSize(mshape_box.GetAABB(), mRes), GetVoxelOffset(mshape_box.GetAABB()));

 	HR(mSwapChain->Present(0, 0));
}

void myDirectX11::resetOMTargetsAndViewport()
{
	//reset rendertarget
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//reset viewport
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)mClientWidth;
	viewport.Height = (float)mClientHeight;
	md3dImmediateContext->RSSetViewports(1, &viewport);
}

//-------------------
//mouse control
//-------------------
void myDirectX11::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void myDirectX11::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(
			0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(
			0.25f*static_cast<float>(y - mLastMousePos.y));
		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void myDirectX11::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);
}

void myDirectX11::ControlCamera(float dt,float speed)
{
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(speed*dt);
	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-speed*dt);
	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-speed*dt);
	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(speed*dt);
	if (GetAsyncKeyState('Q') & 0x8000)
		mCam.FlyVertical(speed*dt);
	if (GetAsyncKeyState('E') & 0x8000)
		mCam.FlyVertical(-speed*dt);
		
}

void myDirectX11::Initvoxel(float res)
{

	mVoxelizer.Init(md3dDevice, md3dImmediateContext,res);

	mVisualizer.Init(md3dDevice, md3dImmediateContext,res);

	mConeTracer.Init(md3dDevice, md3dImmediateContext,res);
}



DirectX::XMFLOAT3 myDirectX11::GetVoxelOffset(DirectX::BoundingBox AABB)
{
	//the vector from min corner to center 
	XMFLOAT3 offset = myMathLibrary::sub(AABB.Center, AABB.Extents);

	//minus -1 to transfom model min corner to zero
	offset = myMathLibrary::multiply(-1.0f, offset);

	return offset;
}



float myDirectX11::GetVoxelSize(DirectX::BoundingBox AABB,float res)
{
	//2*extent/res
	XMFLOAT3 voxelRealSize = myMathLibrary::multiply(2.0f*(1.0f / res), AABB.Extents);

	// Find the maximum component of a voxel.
	float maxVoxelSize = max(voxelRealSize.z, max(voxelRealSize.x, voxelRealSize.y));

	return maxVoxelSize;
}

void myDirectX11::Clear()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


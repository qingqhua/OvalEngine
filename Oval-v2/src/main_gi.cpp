//-----------------------------------
//FILE : main_gi.cpp
//-----------------------------------

#include "core/systemInst.h"

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
	mMainWndCaption = L"box demo";

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
	mWorldInversTrans = myMathLibrary::InverseTranspose(mWorld);

	BuildGeometryBuffer();

	Initvoxel(128.0f);

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
	ControlCamera(dt,5.0f);
	mCam.UpdateViewMatrix();
}


void myDirectX11::DrawScene()
{
 	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
 	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//reset depth/blend state
 	md3dImmediateContext->OMSetDepthStencilState(0, 0);
 	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
 	md3dImmediateContext->OMSetBlendState(0, blendFactors, 0xffffffff);

	//set vertex buffer
	UINT stride = sizeof(myShapeLibrary::Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);

	//set index buffer
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	//send data to compute shader
	//compute_shader.Render(mVoxelizer.SRV(), &mCam.View(), &mCam.Proj(), &mWorld, &mWorldInversTrans, mCam.GetPosition(), mTimer.TotalTime());

	//-----------------------
	//voxelize
	//---------------------
 		mVoxelizer.SetMatrix(&mWorld,&mWorldInversTrans, &mCam.View(), &mCam.Proj(), mCam.GetPosition());
 		mVoxelizer.Render(mTimer.TotalTime());
		md3dImmediateContext->DrawIndexed(indexCount, 0, 0);
		mVoxelizer.Clear();
 		
 		resetOMTargetsAndViewport();
	//-----------------------
	//render visualizer
	//---------------------
	mVisualizer.Render(mVoxelizer.SRV(), &mCam.View(), &mCam.Proj(),&mWorld, &mWorldInversTrans);

	//-----------------------
	//render cone tracing
	//---------------------
 	mConeTracer.SetMatrix(&mWorld, &mWorldInversTrans, &mCam.View(), &mCam.Proj(), mCam.GetPosition());
 	mConeTracer.Render(mVoxelizer.SRV(), mTimer.TotalTime());
 
 	md3dImmediateContext->DrawIndexed(indexCount, 0, 0);

 	HR(mSwapChain->Present(0, 0));
}

void myDirectX11::BuildGeometryBuffer()
{
	Object mObjectCornellBox;
	mObjectCornellBox.LoadModel("data/Model/CornellBox.obj");
	mBoundingBox = mObjectCornellBox.boundingbox();

	unsigned int totalVbd = mObjectCornellBox.verticeByteWidth();
	unsigned int totalIbd = mObjectCornellBox.indiceByteWidth();
	indexCount = mObjectCornellBox.indiceNum();

	//Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = totalVbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &mObjectCornellBox.model.vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = totalIbd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &mObjectCornellBox.model.indiceindex[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
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
	//build voxelizer
	float voxelRes = res;

	//2*extent/res
	XMFLOAT3 voxelRealSize = myMathLibrary::multiply(2.0f*(1.0f / voxelRes), mBoundingBox.Extents);
	// Find the maximum component of a voxel.
	float maxVoxelSize = max(voxelRealSize.z, max(voxelRealSize.x, voxelRealSize.y));
	//the vector from min corner to center 
	XMFLOAT3 offset = myMathLibrary::sub(mBoundingBox.Center, mBoundingBox.Extents); 
	//minus -1 to transfom model min corner to zero
	offset = myMathLibrary::multiply(-1.0f, offset);
	
	mVoxelizer.Init(md3dDevice, md3dImmediateContext, voxelRes, maxVoxelSize, offset);

	mVisualizer.Init(md3dDevice, md3dImmediateContext, mVoxelizer.Res(), mVoxelizer.voxelSize(), offset);
	mConeTracer.Init(md3dDevice, md3dImmediateContext,mVoxelizer.Res(), mVoxelizer.voxelSize(), offset);

	//compute_shader.Init(md3dDevice, md3dImmediateContext, mVoxelizer.Res(), mVoxelizer.voxelSize(), offset);
}



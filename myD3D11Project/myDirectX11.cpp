//-----------------------------------
//FILE:myDirectX11.cpp
//-----------------------------------

#include "myDirectX11.h"

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
	mfxLight(0), mfxMat(0), mfxEyePos(0),
	mTheta(1.5f*3.14f), mPhi(0.25f*3.14f), mRadius(10.0f), mEyePos(0.0f, 0.0f, 0.0f),
	mfxTextureSRV(0), mDiffuseMapSRV(0)
{
	mMainWndCaption = L"box demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMMATRIX Scale = XMMatrixScaling(1.0, 1.0, 1.0);
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(I, Scale));
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
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

	BuildGeometryBuffer();

	mVoxelizer.Init(md3dDevice, md3dImmediateContext, 128);

	XMFLOAT3 voxelRealSize = XMFLOAT3(2.0 * mBoundingBox.Extents.x/ 127.0, 2.0 * mBoundingBox.Extents.y / 127.0, 2.0 * mBoundingBox.Extents.z / 127.0);

	// Find the maximum component of a voxel.
	float m_fVoxelSize = max(voxelRealSize.z, max(voxelRealSize.x, voxelRealSize.y));

	return true;
}

void myDirectX11::OnResize()
{
	D3DApp::OnResize();

	//TODO:
	//...
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*myMathLibrary::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void myDirectX11::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	mEyePos = XMFLOAT3(x, y, z);
}

void myDirectX11::DrawScene()
{
 	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
 	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//set primitive topology
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set vertex buffer
	UINT stride = sizeof(myShapeLibrary::Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);

	//set index buffer
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	//world matrix update
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	mVoxelizer.SetMatrix(&world, &view, &proj);
	mVoxelizer.Render();

	//Update Light
// 	DirectionalLight vLight;
// 	vLight.Direction = XMFLOAT3(0.5f, -0.5f, 0.0f);
// 	vLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.3f, 1.0f);
// 	vLight.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
// 	vLight.Specular = XMFLOAT4(0.0f, 0.3f, 1.0f, 1.0f);
	//mfxLight->SetRawValue(&vLight, 0, sizeof(vLight));

	//Update Material
// 	Material mat;
// 	mat.Diffuse = XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f);
// 	mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
// 	mat.Specular = XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f);
	//mfxMat->SetRawValue(&mat, 0, sizeof(mat));

	//Update eyePos
	//mfxEyePos->SetRawValue(&mEyePos, 0, sizeof(mEyePos));

	//Update Tex
	//mfxTextureSRV->SetResource(mDiffuseMapSRV);


	md3dImmediateContext->DrawIndexed(indexCount, 0, 0);

	HR(mSwapChain->Present(0, 0));
}

void myDirectX11::BuildGeometryBuffer()
{
	myShapeLibrary::MeshData model;
	myShapeLibrary shapes;
	shapes.CreateBox(XMFLOAT3(0, 0, 0), 0.3f, model);

	mBoundingBox = shapes.GetAABB(model);

	//Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(myShapeLibrary::Vertex)*model.vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &model.vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int)*model.indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &model.indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

	indexCount = model.indices.size();
}

void myDirectX11::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void myDirectX11::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = myMathLibrary::Clamp(mPhi, 0.1f, myMathLibrary::Pi*2.0f - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = myMathLibrary::Clamp(mRadius, 3.0f, 15.0f);
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




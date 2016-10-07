#include "myDirectX11.h"

using namespace DirectX;

struct  Vertex
{
	XMFLOAT3 Pos;
	XMVECTORF32 Color;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	myDirectX11 theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

myDirectX11::myDirectX11(HINSTANCE hInstance)
	: D3DApp(hInstance), mBoxVB(0), mBoxIB(0), mFX(0), mTech(0),mfxWorldViewProj(0),
	mInputLayout(0),
	mTheta(1.5f*3.14f), mPhi(0.25f*3.14f), mRadius(10.0f)
{
	mMainWndCaption = L"box demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&m2ndWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	XMMATRIX boxOffest = XMMatrixTranslation(0.0f, 2.0f, -1.0f);
	XMStoreFloat4x4(&m2ndWorld, XMMatrixMultiply(I, boxOffest));
}

myDirectX11::~myDirectX11()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
}						   

bool myDirectX11::Init()
{
	if(!D3DApp::Init())
		return false;

	BuildGeometryBuffer();
	BuildFX();
	BuildVertexLayout();

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

	mfxTime->SetFloat((mTimer.TotalTime()));
}

void myDirectX11::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	
	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		
		/*DrawIndexed(UINT IndexCount,
					UINT StartIndexLocation,
					INT BaseVertexLocation);
		IndexCount:how many indices to use continuously
		StartIndexLocation: the start of index count
		BaseVertexLocation: the start of vertex count

		Attention: the index count and vertex count will not be matched in some ways, 
		for instance, using (36, 36, 8), the index numbers are 8-15, but we should use
		0-7 because there is no 8 in the 0-7 vertex vertices.
		*/

		md3dImmediateContext->DrawIndexed(36,0, 0);
	} 

// 	//draw the second box
	XMMATRIX mRotateY = XMMatrixRotationY(mTimer.TotalTime());
	XMMATRIX mTranslate = XMMatrixTranslation(4.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m2ndWorld, XMMatrixMultiply(mTranslate,mRotateY ));
 	world = XMLoadFloat4x4(&m2ndWorld);
 	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*view*proj)));
 	mTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
 	md3dImmediateContext->DrawIndexed(36, 0, 0);

	HR(mSwapChain->Present(0, 0));
}

void myDirectX11::BuildGeometryBuffer()
{
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), Colors::White },//32byte
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), Colors::Black },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), Colors::Red },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), Colors::Green },
 		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), Colors::Blue },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), Colors::Yellow },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), Colors::Cyan },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), Colors::Magenta }
	};

	//Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	// Create the index buffer
	unsigned int indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		 //back face
		4, 6, 5,
		4, 6, 7,
		 /*left face*/
		0, 4, 5,
		0, 5, 1,
		 /*right face*/
		3, 2, 6,
		3, 2, 7,
		 /*top face*/
		1, 5, 6,
		1, 6, 2,
		 /*bottom face*/
		4, 0, 7,
		0, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(indices) ;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

}

void myDirectX11::BuildFX()
{
	d3dHelper::CreateRandomTexture1DSRV(md3dDevice);
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DCompileFromFile(L"FX/demo.fx", 0, 0, NULL,
		"fx_5_0", shaderFlags,
		0, &compiledShader, &compilationMsgs);
	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	} 
	//Even if there are no compilationMsgs, check to make sure there
		// were no other errors.
		if (FAILED(hr))
		{
			DXTrace(__FILEW__, (DWORD)__LINE__, hr,
				L"D3DX11CompileFromFile", true);
		} HR
		(D3DX11CreateEffectFromMemory(
			compiledShader->GetBufferPointer(),
			compiledShader->GetBufferSize(),
			0, md3dDevice, &mFX));
		// Done with compiled shader.
		ReleaseCOM(compiledShader);
		mTech = mFX->GetTechniqueByName("LightTech");
		mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
		mfxTime = mFX->GetVariableByName("gTime")->AsScalar();
}

void myDirectX11::BuildVertexLayout()
{
	//Create the vertex input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	//create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
	passDesc.IAInputSignatureSize, &mInputLayout));
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




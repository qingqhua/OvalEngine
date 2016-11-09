#include "myDirectX11.h"

using namespace DirectX;

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
	: D3DApp(hInstance), 
	mBoxVB(0), mBoxIB(0), 
	mFX(0), mTech(0), mInputLayout(0),
	mfxWorld(0),mfxProj(0),mfxView(0),mfxLight(0),mfxEyePos(0),
	mfxMatBox(0),mfxMatBackWall(0),mfxMatFloor(0),mEyePos(0.0f,1.0f,0.0f)
{
	mMainWndCaption = L"box demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&m2ndWorld, I);
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
//	BuildTexture();
	BuildVertexLayout();

	SetLight();
	SetMat();

	return true;
}

void myDirectX11::OnResize()
{
	D3DApp::OnResize();
	
	//TODO:
	//...
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	camera.setLens(0.25*myMathLibrary::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void myDirectX11::UpdateScene(float dt)
{
	if (GetAsyncKeyState('W') & 0x8000)
		camera.Walk(10.0f*dt);
	if (GetAsyncKeyState('S') & 0x8000)
		camera.Walk(-10.0f*dt);
	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera.Strafe(-10.0f*dt);
		camera.RotateY(-myMathLibrary::Pi / 2 * dt);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera.Strafe(10.0f*dt);
		camera.RotateY(myMathLibrary::Pi/2*dt);
	}
		

	// Build the view matrix.
	camera.UpdateViewMatrix();

	mEyePos = camera.GetPosition();
}

void myDirectX11::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//set input layout
	md3dImmediateContext->IASetInputLayout(mInputLayout);

	//set primitive topology
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set vertex buffer
	UINT stride = sizeof(myShapeLibrary::Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);

	//set index buffer
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_FLOAT, 0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();

	mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
	mfxView->SetMatrix(reinterpret_cast<float*>(&view));
	mfxProj->SetMatrix(reinterpret_cast<float*>(&proj));

	//Update eyePos
	mfxEyePos->SetRawValue(&mEyePos, 0, sizeof(mEyePos));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	
	//draw the box

	
	for(int i=0;i<1;i++)
		for (int j = 0; j < 1; j++)
			for(int k=0;k<1;k++)
		{
			mfxWorld->SetMatrix(reinterpret_cast<float*>(&(XMMatrixTranslation(i, j, k))));
			
			mTech->GetPassByName("P_Box")->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(36, 0, 0);
		}

	//draw the floor
	XMMATRIX mTranslate = XMMatrixTranslation(0.0f, -0.25f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(4.0f, 0.1f, 4.0f);
	XMMATRIX mworld_back = mScale*mTranslate*mTranslate;
 	mfxWorld->SetMatrix(reinterpret_cast<float*>(&(mworld_back)));
 	mTech->GetPassByName("P_Floor")->Apply(0, md3dImmediateContext);
 	md3dImmediateContext->DrawIndexed(36, 0, 0);

	//draw the back wall
	mTranslate = XMMatrixTranslation(0.0f, 1.5f, 2.0f);
	XMMATRIX mRotateX = XMMatrixRotationX(myMathLibrary::Pi/2);
	XMMATRIX mworld_wall = mScale*mRotateX*mTranslate;
	mfxWorld->SetMatrix(reinterpret_cast<float*>(&(mworld_wall)));
	mTech->GetPassByName("P_BackWall")->Apply(0, md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(36, 0, 0);

	HR(mSwapChain->Present(0, 0));
}

void myDirectX11::BuildGeometryBuffer()
{
	myShapeLibrary::MeshData quad;
	myShapeLibrary shapes;
	shapes.CreateBox( 1.0f,1.0f,1.0f,quad);

	//Create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(myShapeLibrary::Vertex)*quad.vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &quad.vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int)*quad.indices.size() ;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &quad.indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

}

void myDirectX11::SetMat()
{
	//Set Box Material
	Material matBox;
	matBox.Diffuse = XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f);
	matBox.Ambient = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	matBox.Specular = XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f);
	mfxMatBox->SetRawValue(&matBox, 0, sizeof(matBox));

	//Set Back Wall material
	Material matBackWall;
	matBackWall.Diffuse = XMFLOAT4(0.0f, 0.3f, 0.5f, 1.0f);
	matBackWall.Ambient = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	matBackWall.Specular = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
	mfxMatBackWall->SetRawValue(&matBackWall, 0, sizeof(matBackWall));


	//Set Floor material
	Material matFloor;
	matFloor.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	matFloor.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	matFloor.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mfxMatFloor->SetRawValue(&matFloor, 0, sizeof(matFloor));
}

void myDirectX11::SetLight()
{
	DirectionalLight vLight;
	vLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	vLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.3f, 1.0f);
	vLight.Ambient = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vLight.Specular = XMFLOAT4(0.0f, 0.3f, 1.0f, 1.0f);
	mfxLight->SetRawValue(&vLight, 0, sizeof(vLight));
}

void myDirectX11::BuildFX()
{
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

		//get series of variable
		mTech = mFX->GetTechniqueByName("LightTech");
		mfxWorld = mFX->GetVariableByName("worldMatrix")->AsMatrix();
		mfxView = mFX->GetVariableByName("viewMatrix")->AsMatrix();
		mfxProj = mFX->GetVariableByName("projMatrix")->AsMatrix();
		mfxEyePos = mFX->GetVariableByName("eyePos")->AsVector();
		mfxLight = mFX->GetVariableByName("dirLight");
		mfxMatBox = mFX->GetVariableByName("matBox");
		mfxMatBackWall = mFX->GetVariableByName("matBackWall");
		mfxMatFloor = mFX->GetVariableByName("matFloor");

}

void myDirectX11::BuildVertexLayout()
{
	//Create the vertex input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	//create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void myDirectX11::BuildTexture()
{
	//HR(CreateDDSTextureFromFile(md3dDevice, L"Textures/WoodCrate.dds", nullptr, &mDiffuseMapSRV));

	//mfxTextureSRV=mFX->GetVariableByName("diffusemap")->AsShaderResource();
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

		camera.Pitch(dy);
		

	}
// 	else if ((btnState & MK_RBUTTON) != 0)
// 	{
// 		// Make each pixel correspond to 0.005 unit in the scene.
// 		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
// 		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);
// 
// 		// Update the camera radius based on input.
// 		mRadius += dx - dy;
// 
// 		// Restrict the radius.
// 		mRadius = myMathLibrary::Clamp(mRadius, 3.0f, 15.0f);
// 	}


	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void myDirectX11::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);
}




#include "Voxelizer.h"

using namespace DirectX;

Voxelizer::Voxelizer():mInputLayout(0), md3dDevice(0), mDeviceContext(0),
						mTech(0),mTex3D(0),mUAV(0),mSRV(0)
						
{
}

Voxelizer::~Voxelizer()
{

}

void Voxelizer::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float res,float voxelsize,DirectX::XMFLOAT3 voxeloffset)
{
	md3dDevice = device;
	mDeviceContext = deviceContext;

	mWidth = res;
	mHeight = res;
	mDepth = res;
	mRes = res;

	//reset viewport = voxel nums
	//eg. 512x512 for 512^3 voxels
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mViewport.Width = res;
	mViewport.Height = res;

 	mDeviceContext->RSSetViewports(1, &mViewport);

	mVoxelSize = voxelsize;
	mVoxelOffset = voxeloffset;

	BuildFX();
	BuildVertexLayout();
	BuildTexture();
	BuildRenderTarget();
}

void Voxelizer::SetMatrix(const DirectX::XMMATRIX* world, const DirectX::XMMATRIX * worldInverTrans, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMFLOAT3 camPos)
{
	mfxWorld->SetMatrix((float*)(world));
	mfxWorldInverTrans->SetMatrix((float*)(worldInverTrans));
	mfxView->SetMatrix((float*)(view));
	mfxProj->SetMatrix((float*)(proj));

	mfxEyePos->SetFloatVector((float *)&camPos);
}

void Voxelizer::Render(float totalTime, int indexcount)
{
	mDeviceContext->GenerateMips(mSRV);

	mfxUAVColor->SetUnorderedAccessView(mUAV);
	mfxTime->SetFloat(totalTime);

	//set primitive topology
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(mInputLayout);
	mTech->GetPassByName("VoxelizerPass")->Apply(0, mDeviceContext);

	mDeviceContext->DrawIndexed(indexcount, 0, 0);
}

void Voxelizer::resetOMTargetsAndViewport()
{
	mDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Set the viewport.
	mDeviceContext->RSSetViewports(1, &m_viewport);
}

void Voxelizer::Clear()
{
	// cleanup
	ID3D11ShaderResourceView* clear_srvs[] = { nullptr, nullptr, nullptr };
	//mDeviceContext->PSSetShaderResources(0, 1, clear_srvs);
	//mDeviceContext->VSSetShaderResources(0, 1, clear_srvs);

	ID3D11UnorderedAccessView* clear_uavs[] = { nullptr, nullptr };
	//float clear_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//mDeviceContext->ClearUnorderedAccessViewFloat(mUAV, clear_color);
	//mDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 1, clear_uavs, nullptr);
}

void Voxelizer::BuildFX()
{	
	//compile shader
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DX11CompileEffectFromFile(L"src/shader/voxelizer.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, md3dDevice, &mFX, &errorBlob);
	if (FAILED(hr)) { MessageBox(nullptr, (LPCWSTR)errorBlob->GetBufferPointer(), L"error", MB_OK); }

	//get series of variable
	mTech = mFX->GetTechniqueByName("VoxelizerTech");

	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInverTrans = mFX->GetVariableByName("gWorldInverTrans")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();

	mfxTime = mFX->GetVariableByName("gTime")->AsScalar();

 	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsScalar();
	mfxDim = mFX->GetVariableByName("gDim")->AsScalar();
	mfxVoxelOffset = mFX->GetVariableByName("gVoxelOffset")->AsVector();

 	mfxUAVColor = mFX->GetVariableByName("gUAVColor")->AsUnorderedAccessView();
 	
	//light
	mfxPointLight = mFX->GetVariableByName("gPointLight");
	mfxMat = mFX->GetVariableByName("gMat");
	mfxEyePos = mFX->GetVariableByName("gEyePosW")->AsVector();

	//set value
	mfxDim->SetInt(mRes);
	mfxVoxelOffset->SetFloatVector((float *)&mVoxelOffset);
	mfxVoxelSize->SetFloat(mVoxelSize);
}

void Voxelizer::BuildVertexLayout()
{
	//Create the vertex input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByName("VoxelizerPass")->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

void Voxelizer::BuildTexture()
{
	//SET TEXTURE DESC
	D3D11_TEXTURE3D_DESC txDesc;
	txDesc.Width = mWidth;
	txDesc.Height = mHeight;
	txDesc.Depth = mDepth;
	txDesc.MipLevels = log2(mDepth);
	txDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//CREATE TEXTURE3D
	HR(md3dDevice->CreateTexture3D(&txDesc, NULL, &mTex3D));

	//SET UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = txDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAVDesc.Texture3D.FirstWSlice = 0;
	UAVDesc.Texture3D.MipSlice = 0;
	UAVDesc.Texture3D.WSize = mDepth;

	//CREATE UAV
	HR(md3dDevice->CreateUnorderedAccessView(mTex3D, &UAVDesc, &mUAV));

	// SET SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = txDesc.Format;
	SRVDesc.Texture3D.MipLevels = txDesc.MipLevels;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	//CREATE SRV
	HR(md3dDevice->CreateShaderResourceView(mTex3D, &SRVDesc, &mSRV));
}


void Voxelizer::BuildRenderTarget()
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = mWidth;
	textureDesc.Height = mHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = md3dDevice->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = md3dDevice->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = md3dDevice->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = mWidth;
	depthBufferDesc.Height = mHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = md3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = md3dDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);

	// Setup the viewport for rendering.
	m_viewport.Width = (float)mWidth;
	m_viewport.Height = (float)mHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f*DirectX::XM_PI, mWidth / mHeight, 1.0f, 1000.0f);
}

ID3D11ShaderResourceView* Voxelizer::GetSRV()
{
	return mSRV;
}

float Voxelizer::GetRes()
{
	return mRes;
}

float Voxelizer::GetvoxelSize()
{
	return mVoxelSize;
}

DirectX::XMMATRIX Voxelizer::GetProjMatrix()
{
	return m_projectionMatrix;
}




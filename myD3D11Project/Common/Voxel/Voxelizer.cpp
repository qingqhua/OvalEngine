#include "Voxelizer.h"

using namespace DirectX;

Voxelizer::Voxelizer():mInputLayout(0), md3dDevice(0), mDeviceContext(0),
						mTech(0),mTex3D(0),mUAV(0),mSRV(0)
						
{
}

Voxelizer::~Voxelizer()
{

}

void Voxelizer::Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, UINT iRes)
{
	md3dDevice = idevice;
	mDeviceContext = ideviceContext;

	mWidth = iRes;
	mHeight = iRes;
	mDepth = iRes;

	mVoxelSize = XMFLOAT3(0.0157, 0.0157, 0.0157);
	mID = 1.0f;

	BuildFX();
	BuildVertexLayout();
	BuildTexture();

	mfxRes->SetInt(iRes);
	mfxVoxelSize->SetFloatVector((float *)&mVoxelSize);

	//todo if the scene have different models, id should change
	mfxObjID->SetFloat(mID);
}

void Voxelizer::SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj)
{
	mfxWorld->SetMatrix((float*)(iWorld));
	mfxView->SetMatrix((float*)(iView));
	mfxProj->SetMatrix((float*)(iProj));
}

void Voxelizer::Render()
{
	

	const float zero[4] = { 0, 0, 0, 0 };
	//mDeviceContext->ClearUnorderedAccessViewFloat(mUAV, zero);
	//mfxTargetUAV->SetUnorderedAccessView(mUAV);
	//mDeviceContext->OMSetRenderTargets(0, NULL, NULL);

	mDeviceContext->IASetInputLayout(mInputLayout);
	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
}

void Voxelizer::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DCompileFromFile(L"FX/voxelizer.fx", 0, 0, NULL,
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
	mTech = mFX->GetTechniqueByName("VoxelizerTech");

	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();

 	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsVector();
 	//mfxTargetUAV = mFX->GetVariableByName("gTargetUAV")->AsUnorderedAccessView();
 	mfxObjID = mFX->GetVariableByName("gObjectID")->AsScalar();
 	mfxRes = mFX->GetVariableByName("gRes")->AsScalar();
	
}

void Voxelizer::BuildVertexLayout()
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

void Voxelizer::BuildTexture()
{
	//SET TEXTURE DESC
	D3D11_TEXTURE3D_DESC txDesc;
	txDesc.Width = mWidth;
	txDesc.Height = mHeight;
	txDesc.Depth = mDepth;
	txDesc.MipLevels = 1;
	txDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

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
	//todo
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = txDesc.Format;
	SRVDesc.Texture3D.MipLevels = 1;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	//CREATE SRV
	HR(md3dDevice->CreateShaderResourceView(mTex3D,&SRVDesc, &mSRV));
}




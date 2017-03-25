#include "Voxelizer.h"

using namespace DirectX;

Voxelizer::Voxelizer():mInputLayout(0), md3dDevice(0), mDeviceContext(0),
						mTech(0),mTex3D(0),mUAV(0),mSRV(0)
						
{
}

Voxelizer::~Voxelizer()
{

}

void Voxelizer::Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, float iRes,float imaxSize)
{
	md3dDevice = idevice;
	mDeviceContext = ideviceContext;

	mWidth = iRes;
	mHeight = iRes;
	mDepth = iRes;
	mRes = iRes;

	//reset viewport = voxel nums
	//eg. 512x512 for 512^3 voxels
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.Width = iRes;
	mViewport.Height = iRes;
 	mDeviceContext->RSSetViewports(1, &mViewport);

	mVoxelSize = XMFLOAT3(imaxSize, imaxSize, imaxSize);

	BuildFX();
	BuildVertexLayout();
	BuildTexture();

	mfxDim->SetInt(iRes);
	mfxVoxelSize->SetFloatVector((float *)&mVoxelSize);
}

void Voxelizer::SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMFLOAT3 icamPos)
{
	mfxWorld->SetMatrix((float*)(iWorld));
	mfxView->SetMatrix((float*)(iView));
	mfxProj->SetMatrix((float*)(iProj));

	mfxEyePos->SetFloatVector((float *)&icamPos);
}

void Voxelizer::Render(float totalTime)
{
	//update light
	mPointLight.Diffuse = XMFLOAT4(0.2f, 0.2f, 1.3f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.5f, 0.3f, 1.0f, 1.0f);
	mPointLight.Attenuation = XMFLOAT3(0.01f, 0.01f, 0.01f);
	mPointLight.Position = XMFLOAT3(-15.0f*cosf(0.7f*totalTime), 0.0f, -15.0f*sinf(0.7f*totalTime));
	mPointLight.Range = 225.0f;
	mfxPointLight->SetRawValue(&mPointLight, 0, sizeof(mPointLight));

	//Update Material
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mfxMat->SetRawValue(&mMat, 0, sizeof(mMat));

	//set primitive topology
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const float zero[4] = { 0, 0, 0, 0 };
	mDeviceContext->ClearUnorderedAccessViewFloat(mUAV, zero);
	mfxUAVColor->SetUnorderedAccessView(mUAV);
	//mDeviceContext->OMSetRenderTargets(0, NULL, NULL);

	mDeviceContext->IASetInputLayout(mInputLayout);
	mTech->GetPassByName("VoxelizerPass")->Apply(0, mDeviceContext);

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
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();

 	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsVector();
 	mfxUAVColor = mFX->GetVariableByName("gUAVColor")->AsUnorderedAccessView();
 	mfxDim = mFX->GetVariableByName("gDim")->AsScalar();

	//light
	mfxPointLight = mFX->GetVariableByName("gPointLight");
	mfxMat = mFX->GetVariableByName("gMat");
	mfxEyePos = mFX->GetVariableByName("gEyePosW")->AsVector();
	
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
	txDesc.Width = mWidth; //256
	txDesc.Height = mHeight; //256
	txDesc.Depth = mDepth; //256
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
	SRVDesc.Format = txDesc.Format;
	SRVDesc.Texture3D.MipLevels = 1 ;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	//CREATE SRV
	HR(md3dDevice->CreateShaderResourceView(mTex3D,&SRVDesc, &mSRV));
}


ID3D11ShaderResourceView* Voxelizer::SRV()
{
	return mSRV;
}

float Voxelizer::Res()
{
	return mRes;
}



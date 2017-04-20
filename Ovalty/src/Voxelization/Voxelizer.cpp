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
}

void Voxelizer::SetMatrix(const DirectX::XMMATRIX* world, const DirectX::XMMATRIX * worldInverTrans, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMFLOAT3 camPos)
{
	mfxWorld->SetMatrix((float*)(world));
	mfxWorldInverTrans->SetMatrix((float*)(worldInverTrans));
	mfxView->SetMatrix((float*)(view));
	mfxProj->SetMatrix((float*)(proj));

	mfxEyePos->SetFloatVector((float *)&camPos);
}

void Voxelizer::Render(float totalTime)
{
	mDeviceContext->GenerateMips(mSRV);
	//clear
	//float clear_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//mDeviceContext->ClearUnorderedAccessViewFloat(mUAV, clear_color);
	//ID3D11UnorderedAccessView* uav_view[] = { mUAV };
	//mDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, nullptr, nullptr, 0, 1, uav_view, nullptr);
	mfxUAVColor->SetUnorderedAccessView(mUAV);
	mfxTime->SetFloat(totalTime);

	//set primitive topology
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(mInputLayout);
	mTech->GetPassByName("VoxelizerPass")->Apply(0, mDeviceContext);
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


ID3D11ShaderResourceView* Voxelizer::SRV()
{
	return mSRV;
}

float Voxelizer::Res()
{
	return mRes;
}

float Voxelizer::voxelSize()
{
	return mVoxelSize;
}



#include "ConeTracer.h"

using namespace DirectX;

ConeTracer::ConeTracer():mInputLayout(0), md3dDevice(0), mDeviceContext(0),mTech(0),mFX(0),
						mfxProj(0),mfxView(0),mfxWorld(0)					
{}

ConeTracer::~ConeTracer()
{}

void ConeTracer::Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext, float res, float voxelsize, DirectX::XMFLOAT3 offset)
{
	md3dDevice = idevice;
	mDeviceContext = ideviceContext;

	mRes = res;
	mOffset = offset;
	mVoxelSize = voxelsize;

	BuildFX();
	BuildVertexLayout();
}

void ConeTracer::SetMatrix(const DirectX::XMMATRIX* iWorld, const DirectX::XMMATRIX * iWorldInverTrans, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj,const DirectX::XMFLOAT3 icamPos)
{
	mfxWorld->SetMatrix((float*)(iWorld));
	mfxWorldInverTrans->SetMatrix((float*)(iWorldInverTrans));
	mfxView->SetMatrix((float*)(iView));
	mfxProj->SetMatrix((float*)(iProj));

	mfxEyePos->SetFloatVector((float *)&icamPos);
}

void ConeTracer::Render(ID3D11ShaderResourceView* iVoxelList, float totalTime)
{
	//update voxel
	mfxVoxelList->SetResource(iVoxelList);

	//Update eyePos
	mfxEyePos->SetRawValue(&mEyePos, 0, sizeof(mEyePos));

	//set primitive topology
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->IASetInputLayout(mInputLayout);

	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
}

void ConeTracer::BuildFX()
{
	//compile shader
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DX11CompileEffectFromFile(L"src/shader/conetracer.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, md3dDevice, &mFX, &errorBlob);
	if (FAILED(hr)){MessageBox(nullptr, (LPCWSTR)errorBlob->GetBufferPointer(), L"error", MB_OK);}

	//get series of variable
	mTech = mFX->GetTechniqueByName("ConeTracingTech");
	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInverTrans = mFX->GetVariableByName("gWorldInverTrans")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();

	mfxEyePos = mFX->GetVariableByName("gEyePosW")->AsVector();

	//voxel
	mfxVoxelList = mFX->GetVariableByName("gVoxelList")->AsShaderResource();
	mfxDim = mFX->GetVariableByName("gDim")->AsScalar();
	mfxVoxelOffset = mFX->GetVariableByName("gVoxelOffset")->AsVector();
	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsScalar();

	//set voxel value
	mfxVoxelSize->SetFloat((float)(mVoxelSize));
	mfxDim->SetFloat((float)(mRes));
	mfxVoxelOffset->SetFloatVector((float*)&mOffset);
}

void ConeTracer::BuildVertexLayout()
{
	//Create the vertex input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByName("ConeTracingPass")->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}


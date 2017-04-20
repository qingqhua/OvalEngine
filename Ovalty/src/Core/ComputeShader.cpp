#include "ComputeShader.h"
using namespace DirectX;


Compute::Compute()
{

}

Compute::~Compute()
{

}

void Compute::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,float res, float voxelsize, DirectX::XMFLOAT3 offset)
{
	md3dDevice = device;
	mDeviceContext = deviceContext;

	mRes = res;
	mOffset = offset;
	mVoxelSize = voxelsize;

	BuildFX();
}

void Compute::Render(ID3D11ShaderResourceView* voxelList, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMMATRIX * world, const DirectX::XMMATRIX * worldInverTrans, const DirectX::XMFLOAT3 camPos, float totalTime)
{
	//update voxel list,defined in "voxelizer.fx"
	//mfxVoxelList->SetResource(voxelList);

	//update matrix
	mfxView->SetMatrix((float*)(view));
	mfxProj->SetMatrix((float*)(proj));
	mfxWorld->SetMatrix((float*)(world));
	mfxWorldInverTrans->SetMatrix((float*)(worldInverTrans));
	mfxEyePosW->SetFloatVector((float*)(&camPos));

	//update time
	mfxTime->SetFloat(totalTime);

	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
}

void Compute::BuildFX()
{
	//compile shader
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DX11CompileEffectFromFile(L"src/shader/compute.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, md3dDevice, &mFX, &errorBlob);
	if (FAILED(hr)) { MessageBox(nullptr, (LPCWSTR)errorBlob->GetBufferPointer(), L"error", MB_OK); }

	//get series of variable
	mTech = mFX->GetTechniqueByName("ComputeTech");

	mfxView = mFX->GetVariableByName("c_View")->AsMatrix();
	mfxProj = mFX->GetVariableByName("c_Proj")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("c_World")->AsMatrix();
	mfxWorldInverTrans = mFX->GetVariableByName("c_WorldInverTrans")->AsMatrix();
	mfxEyePosW = mFX->GetVariableByName("c_EyePosW")->AsVector();

	//mfxVoxelList = mFX->GetVariableByName("c_VoxelList")->AsShaderResource();

	mfxVoxelSize = mFX->GetVariableByName("c_VoxelSize")->AsScalar();
	mfxDim = mFX->GetVariableByName("c_Dim")->AsScalar();
	mfxVoxelOffset = mFX->GetVariableByName("c_VoxelOffset")->AsVector();

	mfxTime = mFX->GetVariableByName("c_Time")->AsScalar();

	//set voxel value
	mfxVoxelSize->SetFloat((float)(mVoxelSize));
	mfxDim->SetFloat((float)(mRes));
	mfxVoxelOffset->SetFloatVector((float*)&mOffset);
}
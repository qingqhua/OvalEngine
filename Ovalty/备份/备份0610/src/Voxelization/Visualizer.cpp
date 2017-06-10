#include "Visualizer.h"
using namespace DirectX;

Visualizer::Visualizer()
{

}

Visualizer::~Visualizer()
{

}

void Visualizer::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,float res, float voxelsize, DirectX::XMFLOAT3 offset)
{
	md3dDevice = device;
	mDeviceContext = deviceContext;

	mRes = res;
	mOffset = offset;
	mVoxelSize = voxelsize;

	BuildFX();
}

void Visualizer::Render(ID3D11ShaderResourceView* voxelList, const DirectX::XMMATRIX* view, const DirectX::XMMATRIX * proj, const DirectX::XMMATRIX * world, const DirectX::XMMATRIX * worldInverTrans)
{
	//update data in "voxelizer.fx"
	mfxVoxelList->SetResource(voxelList);

	//update matrix
	mfxView->SetMatrix((float*)(view));
	mfxProj->SetMatrix((float*)(proj));
	mfxWorld->SetMatrix((float*)(world));
	mfxWorldInverTrans->SetMatrix((float*)(worldInverTrans));

	mDeviceContext->IASetInputLayout(mInputLayout);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
 	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
 	int voxelNum = mRes*mRes*mRes;
	mDeviceContext->Draw(voxelNum,0);
}

void Visualizer::BuildFX()
{
	//compile shader
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DX11CompileEffectFromFile(L"src/shader/Visualizer.fx", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, md3dDevice, &mFX, &errorBlob);
	if (FAILED(hr)) { MessageBox(nullptr, (LPCWSTR)errorBlob->GetBufferPointer(), L"error", MB_OK); }


	//get series of variable
	mTech = mFX->GetTechniqueByName("VisualTech");

	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInverTrans = mFX->GetVariableByName("gWorldInverTrans")->AsMatrix();

	mfxEdgeTex = mFX->GetVariableByName("gEdge")->AsShaderResource();
	mfxVoxelList = mFX->GetVariableByName("gVoxelList")->AsShaderResource();
	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsScalar();
	mfxDim = mFX->GetVariableByName("gDim")->AsScalar();
	mfxVoxelOffset = mFX->GetVariableByName("gVoxelOffset")->AsVector();

	// Set a texture for voxels.
	ID3D11ShaderResourceView* edgeTexRV;
	HR(CreateDDSTextureFromFile(md3dDevice, L"data/Texture/WoodCrate.dds", nullptr, &edgeTexRV));
	mfxEdgeTex->SetResource(edgeTexRV);

	//set voxel value
	mfxVoxelSize->SetFloat((float)(mVoxelSize));
	mfxDim->SetFloat((float)(mRes));
	mfxVoxelOffset->SetFloatVector((float*)&mOffset);
}
#include "Visualizer.h"
using namespace DirectX;

Visualizer::Visualizer()
{

}

Visualizer::~Visualizer()
{

}

void Visualizer::Init(ID3D11Device* idevice, ID3D11DeviceContext* ideviceContext)
{
	md3dDevice = idevice;
	mDeviceContext = ideviceContext;

	BuildFX();
	//BuildVertexLayout();
}

void Visualizer::Render(ID3D11ShaderResourceView* iVoxelList, float iRes, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMMATRIX * iWorld, const DirectX::XMMATRIX * iWorldInverTrans)
{
	//update data in "voxelizer.fx"
	mfxVoxelList->SetResource(iVoxelList);
	//update voxel size
	float fScale = 10.0f;
	float voxelScale = fScale / iRes;
	mfxVoxelSize->SetFloat((float)(voxelScale));

	//update matrix
	mfxView->SetMatrix((float*)(iView));
	mfxProj->SetMatrix((float*)(iProj));
	mfxWorld->SetMatrix((float*)(iWorld));
	mfxWorldInverTrans->SetMatrix((float*)(iWorldInverTrans));

	mDeviceContext->IASetInputLayout(mInputLayout);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
 	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
 	int voxelNum = iRes*iRes*iRes;
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

	// Set a texture for voxels.
	ID3D11ShaderResourceView* edgeTexRV;
	HR(CreateDDSTextureFromFile(md3dDevice, L"data/Texture/WoodCrate.dds", nullptr, &edgeTexRV));
	mfxEdgeTex->SetResource(edgeTexRV);
}

void Visualizer::BuildVertexLayout()
{
	//Create the vertex input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 1, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}

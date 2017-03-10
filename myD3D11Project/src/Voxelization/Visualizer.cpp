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

void Visualizer::Render(ID3D11ShaderResourceView* iVoxelList, float iRes, const DirectX::XMMATRIX* iView, const DirectX::XMMATRIX * iProj, const DirectX::XMMATRIX * iWorld)
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

	mDeviceContext->IASetInputLayout(mInputLayout);
  	//mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
 	mTech->GetPassByIndex(0)->Apply(0, mDeviceContext);
 	int voxelNum = iRes*iRes*iRes;
	mDeviceContext->Draw(voxelNum,0);
}

void Visualizer::BuildFX()
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DCompileFromFile(L"src/shader/Visualizer.fx", 0, 0, NULL,
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
			L"ERROR: D3DCompileFromFile", true);
	} HR
	(D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));
	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	//get series of variable
	mTech = mFX->GetTechniqueByName("VisualTech");

	mfxView = mFX->GetVariableByName("gView")->AsMatrix();
	mfxProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();

	mfxEdgeTex = mFX->GetVariableByName("gEdge")->AsShaderResource();
	mfxVoxelList = mFX->GetVariableByName("gVoxelList")->AsShaderResource();
	mfxVoxelSize = mFX->GetVariableByName("gVoxelSize")->AsScalar();

	// Set a texture for voxels.
	ID3D11ShaderResourceView* edgeTexRV;
	if (FAILED(hr))
	{
		DXTrace(__FILEW__, (DWORD)__LINE__, hr,L"ERROR: CreateDDSTextureFromFile", true);
	} 
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

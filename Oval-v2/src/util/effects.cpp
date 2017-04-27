#include "effects.h"

Effect::Effect()
{

}

Effect::~Effect()
{

}

void Effect::Init(ID3D11Device* device, LPCWSTR filename)
{
	Compile(device, filename);

	SetTechParamter();
}

void Effect::Render(ID3D11DeviceContext* context)
{
	// Set the vertex input layout, default is pos, normal, texture.
	context->IASetInputLayout(m_InputLayout);

	//apply tech
	m_Tech->GetPassByIndex(0)->Apply(0, context);
}

void Effect::Shutdown()
{

}

//---------------------
//Init
//-----------------------
void Effect::Compile(ID3D11Device* device, LPCWSTR filename)
{
	//compile shader
	ID3DBlob* errorBlob;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DX11CompileEffectFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags,
		0, device, &m_FX, &errorBlob);
	if (FAILED(hr)) { MessageBox(nullptr, (LPCWSTR)errorBlob->GetBufferPointer(), L"error", MB_OK); }
}

void Effect::SetTex3dUAV(ID3D11Device* device, float width)
{
	//---
	//Setup the texture 3d description.
	//---
	D3D11_TEXTURE3D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = textureDesc.Width;
	textureDesc.Depth = textureDesc.Width;
	textureDesc.MipLevels = log2(textureDesc.Width);
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//Create texture 3d.
	device->CreateTexture3D(&textureDesc, NULL, &m_Tex3D);

	//---
	//Setup the unordered access view description.
	//---
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = textureDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAVDesc.Texture3D.FirstWSlice = 0;
	UAVDesc.Texture3D.MipSlice = 0;
	UAVDesc.Texture3D.WSize = textureDesc.Width;

	//Bind Tex3D to Unordered Access View
	device->CreateUnorderedAccessView(m_Tex3D, &UAVDesc, &m_UAV);

	//---
	//Setup the shader resource view description.
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = textureDesc.Format;
	SRVDesc.Texture3D.MipLevels = textureDesc.MipLevels;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	//Bind Tex3D to shader resource view
	device->CreateShaderResourceView(m_Tex3D, &SRVDesc, &m_SRV);

	m_fxUAV = m_FX->GetVariableByName("uav_color")->AsUnorderedAccessView();
}

void Effect::CreateInputLayout(ID3D11Device* device)
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
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);

	device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_InputLayout);
}


//--------------------------------
//Set Paramters
//--------------------------------
void Effect::SetTechParamter()
{
	m_Tech = m_FX->GetTechniqueByIndex(0);
}

void Effect::SetMatrixParamter()
{
	m_cbMatrix = m_FX->GetVariableByName("cb_Matrix");
}

void Effect::SetSRVList()
{
	m_fxSRVList=m_FX->GetVariableByName("uav_color")->AsShaderResource();
}

void Effect::SetCommonParamter()
{
	m_cbCommon = m_FX->GetVariableByName("cb_Common");
}

void Effect::SetUpdateVoxelParameter(float dim, const DirectX::BoundingBox* AABB)
{
	m_cbVoxel = m_FX->GetVariableByName("cb_Voxel");

	//2*extent/res
	DirectX::XMFLOAT3 voxelRealSize;
	voxelRealSize.x = 2.0f*(1.0f / dim)* AABB->Extents.x;
	voxelRealSize.y = 2.0f*(1.0f / dim)* AABB->Extents.y;
	voxelRealSize.z = 2.0f*(1.0f / dim)* AABB->Extents.z;

	// Find the maximum component of a voxel.
	float maxVoxelSize = max(voxelRealSize.z, max(voxelRealSize.x, voxelRealSize.y));

	//the vector from min corner to center 
	//minus -1 to transfom model min corner to zero
	DirectX::XMFLOAT3 offset;
	offset.x = -(AABB->Center.x - AABB->Extents.x);
	offset.y = -(AABB->Center.y - AABB->Extents.y);
	offset.z = -(AABB->Center.z - AABB->Extents.z);

	VoxelBufferType voxel;

	voxel.dimension = dim;
	voxel.offset = offset;
	voxel.size = maxVoxelSize;

	m_cbVoxel->SetRawValue(&voxel, 0, sizeof(VoxelBufferType));
}

//----------
//Update
//----------
void Effect::UpdateMatrixParameters(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj)
{
	MatrixBufferType matrix;
	matrix.world = *world;
	matrix.view = *view;
	matrix.projection = *proj;

	m_cbMatrix->SetRawValue(&matrix, 0, sizeof(MatrixBufferType));
}

void Effect::UpdateCommonParameters(float time, DirectX::XMFLOAT3 eyeposW)
{
	CommonBufferType common;
	common.eyeposW = eyeposW;
	common.time = time;

	m_cbCommon->SetRawValue(&common, 0, sizeof(CommonBufferType));
}

void Effect::UpdateUAVParameters()
{
	m_fxUAV->SetUnorderedAccessView(m_UAV);
}

void Effect::UpdateSRVParamters(ID3D11ShaderResourceView* srvList)
{
	m_fxSRVList->SetResource(srvList);
}

ID3D11ShaderResourceView* Effect::GetSRV()
{
	return m_SRV;
}

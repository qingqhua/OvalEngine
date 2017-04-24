//-----------------------------------
//FILE : voxelization_shader.cpp
//-----------------------------------

#include "voxelization_shader.h"

VoxelizationShader::VoxelizationShader()
{

}

VoxelizationShader::~VoxelizationShader()
{

}

bool VoxelizationShader::Init(ID3D11Device* device, HWND hwnd, WCHAR *vs, WCHAR *ps, WCHAR *gs)
{
	bool result;

	result = m_shader_tools.Compile(device, hwnd, vs, ps, gs);
	result = m_shader_tools.SetConstantBuffer(device);
	result = m_shader_tools.SetSampler(device);

	if (!result)
	{
		return false;
	}

	return true;
}

void VoxelizationShader::Shutdown()
{
	m_shader_tools.Shutdown();
}

bool VoxelizationShader::Render(ID3D11DeviceContext* context, int indexCount, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, const ID3D11ShaderResourceView* srv)
{
	bool result;

	//set parameter
	result = m_shader_tools.UpdateShaderParameters(context, indexCount, world, view, proj, NULL);
	if (!result)
	{
		return false;
	}

	return true;
}
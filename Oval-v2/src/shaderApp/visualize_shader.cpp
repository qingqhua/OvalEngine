//-----------------------------------
//FILE : voxelization_shader.cpp
//-----------------------------------

#include "voxelization_shader.h"

VoxelizationShader::VoxelizationShader()
	:m_dimension(256.0f)
{

}

VoxelizationShader::~VoxelizationShader()
{

}

bool VoxelizationShader::Init(ID3D11Device* device, LPCWSTR filename,float dimension, const DirectX::BoundingBox* AABB)
{
	m_dimension = dimension;

	m_effect.Init(device, filename);
	m_effect.SetMatrixParamter();
	m_effect.SetCommonParamter();
	m_effect.SetUpdateVoxelParameter(m_dimension, AABB);
	m_effect.SetTex3dUAV(device, m_dimension);

	return true;
}

void VoxelizationShader::Shutdown()
{
	m_effect.Shutdown();
}

void VoxelizationShader::Render(ID3D11DeviceContext* context, int indexCount, 
								const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj,
								float time,DirectX::XMFLOAT3 eyeposw)
{
	m_effect.ResetViewport(context, m_dimension, m_dimension);
	m_effect.UpdateUAVParameters();
	m_effect.UpdateCommonParameters(time, eyeposw);
	m_effect.UpdateMatrixParameters(world, view, proj);
	m_effect.Update(context, indexCount);
	m_effect.ResetViewport(context, 800,600);
}
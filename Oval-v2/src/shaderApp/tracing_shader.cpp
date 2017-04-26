//-----------------------------------
//FILE : tracing_shader.cpp
//-----------------------------------

#include "tracing_shader.h"

TracingShader::TracingShader()
	:	m_dimension(256.0f)
{

}

TracingShader::~TracingShader()
{

}

bool TracingShader::Init(ID3D11Device* device, LPCWSTR filename,float dimension, const DirectX::BoundingBox* AABB)
{
	m_dimension = dimension;

	//compile shader
	m_effect.Init(device, filename);
	//create per vertex input layout
	m_effect.CreateInputLayout(device);

	//update constant buffer
	m_effect.SetMatrixParamter();
	m_effect.SetCommonParamter();
	m_effect.SetUpdateVoxelParameter(m_dimension, AABB);

	//set srv texture to load saved voxel 3d texture
	m_effect.SetSRVList();

	return true;
}

void TracingShader::Shutdown()
{
	m_effect.Shutdown();
}

void TracingShader::Render(ID3D11DeviceContext* context, int indexCount, 
								const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj,
								float time,DirectX::XMFLOAT3 eyeposw,
							ID3D11ShaderResourceView *srvList)
{
	//update constant buffer
	m_effect.UpdateCommonParameters(time, eyeposw);
	m_effect.UpdateMatrixParameters(world, view, proj);
	m_effect.UpdateSRVParamters(srvList);

	//apply shader
	m_effect.Render(context);

	//draw model with this shader
	context->DrawIndexed(indexCount, 0, 0);
}
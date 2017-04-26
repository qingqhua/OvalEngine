//-----------------------------------
//FILE			: tracing_shader.h
//DESCRIPTION	: setup cone tracing shader
//PARENT		: GRAPHICS class 
//NODE			: effects
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef TRACING_SHADER_H_
#define TRACING_SHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "effects.h"

class TracingShader
{
public:
	TracingShader();
	~TracingShader();
	bool Init(ID3D11Device* device, LPCWSTR filename,float dimension, const DirectX::BoundingBox* AABB);
	void Render(ID3D11DeviceContext* context, int indexCount, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, float time, DirectX::XMFLOAT3 eyeposw, ID3D11ShaderResourceView *srvList);
	void Shutdown();

private:
	float m_dimension;
	Effect m_effect;
};

#endif //VOXELIZATION_SHADER_H_
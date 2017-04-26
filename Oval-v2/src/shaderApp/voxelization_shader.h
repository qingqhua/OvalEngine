//-----------------------------------
//FILE			: voxelization_shader.h
//DESCRIPTION	: setup voxelization shader
//PARENT		: GRAPHICS class 
//NODE			: effects
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef VOXELIZATION_SHADER_H_
#define VOXELIZATION_SHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "effects.h"

class VoxelizationShader
{
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
public:
	VoxelizationShader();
	~VoxelizationShader();
	bool Init(ID3D11Device* device, LPCWSTR filename,float dimension, const DirectX::BoundingBox* AABB);
	void Render(ID3D11DeviceContext* context, int indexCount, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, float time, DirectX::XMFLOAT3 eyeposw);
	void Shutdown();

	ID3D11ShaderResourceView* GetSRV();

private:
	float m_dimension;
	Effect m_effect;
};

#endif //VOXELIZATION_SHADER_H_
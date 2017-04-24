//-----------------------------------
//FILE			: voxelization_shader.h
//DESCRIPTION	: setup voxelization shader
//PARENT		: GRAPHICS class 
//NODE			: shader_tools
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef VOXELIZATION_SHADER_H_
#define VOXELIZATION_SHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "shader_tools.h"
class VoxelizationShader
{

public:
	VoxelizationShader();
	~VoxelizationShader();
	bool Init(ID3D11Device* device, HWND hwnd, WCHAR *vs, WCHAR *ps, WCHAR *gs);
	bool Render(ID3D11DeviceContext* context, int indexCount, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, const ID3D11ShaderResourceView* srv);
	void Shutdown();

private:
	Shader m_shader_tools;
};

#endif //VOXELIZATION_SHADER_H_
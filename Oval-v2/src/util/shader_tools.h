//-----------------------------------
//FILE			: shader_tools.h
//DESCRIPTION	: compile shader
//PARENT		: shaderApp class 
//NODE			: EMPTY
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef SHADER_TOOLS_H_
#define SHADER_TOOLS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class Shader
{
public:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	Shader();
	~Shader();
	bool Compile(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs);
	bool SetSampler(ID3D11Device* device);
	bool SetConstantBuffer(ID3D11Device* device);

	bool UpdateShaderParameters(ID3D11DeviceContext* context, int indexCount, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, const ID3D11ShaderResourceView* srv);
	
	void Shutdown();

private:
	bool InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs);

	bool UpdateParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj, const ID3D11ShaderResourceView* srv);
	void RenderShader(ID3D11DeviceContext* context, int indexcount);

	void ShutdownShader();

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif //VOXELIZATION_SHADER_H_
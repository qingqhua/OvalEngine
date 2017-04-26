//-----------------------------------
//FILE			: shader_tools.h
//DESCRIPTION	: compile shader
//PARENT		: shaderApp class 
//NODE			: EMPTY
//REFERENCE		: d3dCoder.net
//-----------------------------------
#ifndef SHADER_TOOLS_H_
#define SHADER_TOOLS_H_


#include "d3dx11effect.h"
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

	struct MaterialBufferType
	{
		DirectX::XMFLOAT3 ka;
		DirectX::XMFLOAT3 kd;
		DirectX::XMFLOAT3 ks;
	};


public:
	Shader();
	~Shader();
	bool Compile(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs);
	bool CreateSampler(ID3D11Device* device);
	bool CreateConstantBuffer(ID3D11Device* device);
	bool CreateTex3dUAV(ID3D11Device* device, float width);
	bool CreateTex2dSRV(ID3D11Device* device);
	void CreateDepthBuffer(ID3D11Device* device);

	void UpdateRenderTarget(ID3D11DeviceContext* context);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a);
	void UpdateMatrixParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj);
	void UpdateUAVParameters(ID3D11DeviceContext* context);
	void UpdateSRVParameters(ID3D11DeviceContext* context);
	void UpdateSamplerParameters(ID3D11DeviceContext* context);

	void Render(ID3D11DeviceContext* context, int indexcount);
	void Shutdown();

	ID3D11ShaderResourceView* GetSRV();

private:
	bool InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs);

	void RenderShader(ID3D11DeviceContext* context, int indexcount);

	void ShutdownShader();

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	ID3D11Texture3D* m_Tex3D;
	ID3D11ShaderResourceView* m_SRV;
	ID3D11UnorderedAccessView* m_UAV;

	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RenderTargetView* m_renderTargetView;
};

#endif //VOXELIZATION_SHADER_H_
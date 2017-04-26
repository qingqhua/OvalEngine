//-----------------------------------
//FILE : texture_tools.cpp
//-----------------------------------

#include "texture_tools.h"

Texture::Texture()
	:	m_Tex3D(0),
		m_Tex2D(0),
		m_SRV(0),
		m_UAV(0),
		m_renderTargetView(0),
		m_depthStencilView(0),
{
}

Texture::~Texture()
{

}



bool Texture::Init()
{
	HRESULT result;


	// Load the texture in.
	result = d3d11createshader(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::SetTex3dUAV(ID3D11Device* device, float width)
{

}

void Texture::SetTex2dSRV(ID3D11Device* device,int width,int height,char *texturename)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	device->CreateTexture2D(&textureDesc, NULL, &m_Tex2D);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	device->CreateShaderResourceView(m_Tex2D, &srvDesc, &m_SRV);
}

void Texture::SetRenderTarget(ID3D11DeviceContext* context)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Set the viewport.
	context->RSSetViewports(1, &m_viewport);
}

void Texture::ClearRenderTargetDepth(ID3D11DeviceContext* context, float r, float g, float b, float a)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	context->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Texture::Shutdown()
{
	if (m_Tex3D)
	{
		delete(m_Tex3D);
		m_Tex3D = 0;
	}

	if (m_Tex2D)
	{
		delete(m_Tex2D);
		m_Tex2D = 0;
	}

	if (m_SRV)
	{
		delete(m_SRV);
		m_SRV = 0;
	}

	if (m_UAV)
	{
		delete(m_UAV);
		m_UAV = 0;
	}

	if (m_renderTargetView)
	{
		delete(m_renderTargetView);
		m_renderTargetView = 0;
	}

	if (m_depthStencilView)
	{
		delete(m_depthStencilView);
		m_depthStencilView = 0;
	}
}

ID3D11ShaderResourceView* Texture::GetSRV()
{
	return m_SRV;
}

ID3D11UnorderedAccessView* Texture::GetUAV()
{
	return m_UAV;
}


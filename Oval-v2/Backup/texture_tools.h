//-----------------------------------
//FILE			: texture_tools.h
//DESCRIPTION	: load texture
//PARENT		: Graphics class 
//NODE			: EMPTY
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef TEXTURE_TOOLS_H_
#define TEXTURE_TOOLS_H_

class Texture
{
public:
	Texture();
	~Texture();
	bool Init();

	void Shutdown();

	ID3D11ShaderResourceView* GetSRV();

private:

private:
	ID3D11Texture3D* m_Tex3D;
	ID3D11Texture2D* m_Tex2D;
	ID3D11ShaderResourceView* m_SRV;
	ID3D11UnorderedAccessView* m_UAV;

	ID3D11RenderTargetView* m_renderTargetView;

	ID3D11DepthStencilView* m_depthStencilView;

	D3D11_VIEWPORT m_viewport;
};

#endif //TEXTURE_TOOLS_H_
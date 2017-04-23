//-----------------------------------
//FILE			: d3dApp.h
//DESCRIPTION	: Direct3D Setup
//PARENT		: graphics class 
//NODE			: EMPTY
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <DirectXMath.h>

class D3DApp
{
public:
	D3DApp();
	~D3DApp();

	bool Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd);
	void Shutdown();

	// beginning of each frame
	void ClearBuffer(float r, float g, float b, float a);
	
	// end of each frame
	void PresentBuffer();

	// get device
	ID3D11Device *Device();
	ID3D11DeviceContext *DeviceContext();

	// returns name of the video card and the amount of video memory
	void GetVideoCardInfo(char *cardname, int &memory);

	void SetBackBufferRenderTarget();
	void ResetViewport(float width, float height);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	DirectX::XMMATRIX m_projmatrix;
	DirectX::XMMATRIX m_worldmatrix;
	DirectX::XMMATRIX m_orthomatrix;

	D3D11_VIEWPORT m_viewport;
};

#endif // D3DAPP_H
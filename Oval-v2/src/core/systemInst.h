//-----------------------------------
//FILE : systemInst.h
//DESCRIPTION : System Instance
//REFERENCE : d3dcoder.net, rastertek.com
//-----------------------------------
#ifndef SYSTEM_INST_H
#define SYSTEM_INST_H

#include <windows.h>


class D3DApp
{
public:
	D3DApp();
	~D3DApp();

	bool Init();
	int Run();
	void Shutdown();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool FrameEvent();
	bool InputEvent(float dt);
	bool InitMainWindow();
	bool InitDirect3D();
	void ShutdownWindow();

private:
	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	float	  mClientWidth;
	float     mClientHeight;
};

class D3DApp1
{
public:
	D3DApp(HINSTANCE hInstance);
	~D3DApp();
	
	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;
	
	int Run();
 
	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0; 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	UINT      m4xMsaaQuality;

	GameTimer mTimer;

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriverType;
	bool mEnable4xMsaa;
};

#endif // D3DAPP_H
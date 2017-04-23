//-----------------------------------
//FILE			: systemInst.h
//DESCRIPTION	: System Windows Instance
//PARENT		: EMPTY 
//NODE			: graphics class, input, model class
//REFERENCE		: d3dcoder.net, rastertek.com
//-----------------------------------
#ifndef SYSTEM_INST_H
#define SYSTEM_INST_H

#include <windows.h>

#include "graphics.h"

class System
{
public:
	System();
	~System();

	bool Init();
	int  Run();
	bool Shutdown();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool InitMainWindow();    
	bool ShutdownWindow();
	bool Update();
	bool InputEvent(float dt);

private:
	HINSTANCE m_hAppInst;
	HWND      m_hMainWnd;
	LPCWSTR   m_MainWndCaption;
	long	  m_ClientWidth;
	long      m_ClientHeight;

	Graphics *m_Graphics;
};

#endif // SYSTEM_INST_H
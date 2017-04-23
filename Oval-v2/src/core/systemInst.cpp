//-----------------------------------
//FILE : systemInst.cpp
//-----------------------------------

#include "systemInst.h"

namespace
{
	System* gSystem = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
		return gSystem->MsgProc(hwnd, msg, wParam, lParam);
}

System::System()
:	m_hAppInst(NULL),
	m_hMainWnd(0),
	m_MainWndCaption(L"OVAL-ENGINE"),
	m_ClientWidth(800),
	m_ClientHeight(600)
{
}

System::~System()
{
}

bool System::Init()
{
	//init window
	if (!InitMainWindow())
		return false;

	//create graphics object
	m_Graphics = new Graphics;
	if (!m_Graphics)
	{
		return false;
	}

	m_Graphics->Init(m_ClientWidth, m_ClientHeight, m_hMainWnd);

	return true;
}

int System::Run()
{
	MSG msg = { 0 };

	bool done = false;

	while (!done)
	{
		// If there are Window messages, process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//quit
		if (msg.message == WM_QUIT)
		{
			done = true;
		}

		else
		{
			if (!Update())
				done = true;
		}
	}

	return (int)msg.wParam;
}

bool System::Shutdown()
{
	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release windows every time.
	return ShutdownWindow();
}

LRESULT System::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool System::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"OvalEngine-qingqHua";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"OvalEngine-qingqHua", m_MainWndCaption,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	ShowCursor(true);

	return true;
}

bool System::ShutdownWindow()
{
	DestroyWindow(m_hMainWnd);
	m_hMainWnd = NULL;

	UnregisterClass(m_MainWndCaption, m_hAppInst);
	m_hAppInst = NULL;

	gSystem = NULL;

	return true;
}

bool System::Update()
{
	bool result;

	result = m_Graphics->Update();
	if (!result)
	{
		return false;
	}

	return true;
}

bool System::InputEvent(float dt)
{
	return true;
}





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
	m_ClientHeight(600),
	m_input(0),
	m_cam(0),
	m_Graphics(0)
{
	gSystem = this;
}

System::~System()
{
}

bool System::Init()
{
	//init window
	InitMainWindow();
		

	//---
	//Create the Input object.
	//---
	m_input = new Input();
	if (!m_input)
	{
		return false;
	}

	m_input->Init(m_hAppInst, m_hMainWnd, m_ClientWidth, m_ClientHeight);

	//---
	//Create the camera object.
	//---
	m_cam = new camera();

	// init position of camera
	m_cam->Init(m_ClientWidth, m_ClientHeight);

	if (!m_cam)
	{
		return false;
	}

	//---
	//Create graphics object.
	//---
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

	// Release the camera object.
	if (m_cam)
	{
		delete m_cam;
		m_cam = 0;
	}

	// Release the graphics object.
	if (m_input)
	{
		m_input->Shutdown();
		delete m_input;
		m_input = 0;
	}

	// Release windows every time.
	return ShutdownWindow();
}

LRESULT System::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
}

bool System::InitMainWindow()
{
	m_hAppInst = GetModuleHandle(NULL);

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"OvalEngine-qingqHua";
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindowEx(WS_EX_APPWINDOW, L"OvalEngine-qingqHua", L"OvalEngine-qingqHua",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		width / 2, height / 2, width, height, NULL, NULL, m_hAppInst, NULL);

	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	SetForegroundWindow(m_hMainWnd);

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

	//update input, read keyboard, mouse
	result = m_input->Update();
	if (!result)
	{
		return false;
	}

	//handle input event
	result = InputEvent(0.5, 0.001);
	if (!result)
	{
		return false;
	}

	//update camera
	result = m_cam->Update();
	if (!result)
	{
		return false;
	}

	//update graphics
	result = m_Graphics->Update(&m_cam->GetWorld(),&m_cam->GetView(),&m_cam->GetProj(),0,m_cam->GetPosition());
	if (!result)
	{
		return false;
	}

	return true;
}

bool System::InputEvent(float speed,float dt)
{
	if(m_input->IsLeftPressed())
		m_cam->Strafe(speed*dt);

	if(m_input->IsRightPressed())
		m_cam->Strafe(-speed*dt);

	if(m_input->IsForwardPressed())
		m_cam->Walk(speed*dt);

	if(m_input->IsBackPressed())
		m_cam->Walk(-speed*dt);

	if(m_input->IsUpPressed())
		m_cam->FlyVertical(speed*dt);

	if(m_input->IsDownPressed())
		m_cam->FlyVertical(-speed*dt);

	return true;
}





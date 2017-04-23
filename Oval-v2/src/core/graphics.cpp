//---------------------------------- -
//FILE : graphics.cpp
//-----------------------------------

#include "graphics.h"

Graphics::Graphics()
	: m_D3DApp(0),
	  m_cam(0)
{
}

Graphics::~Graphics()
{

}

bool Graphics::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	bool VSYNC_ENABLED = true;

	//---
	//Create the Direct3D object.
	//---
	m_D3DApp = new D3DApp;
	if (!m_D3DApp)
	{
		return false;
	}

	// Init Direct3D object.
	result = m_D3DApp->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	//---
	//Create the camera object.
	//---
	m_cam = new camera;

	// set init position of camera
	m_cam->SetPosition(0.0f, 0.0f, -5.0f);

	if (!m_cam)
	{
		MessageBox(hwnd, L"Could not initialize Camera.", L"Error", MB_OK);
		return false;
	}



	return true;
}

bool Graphics::Update()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	// Release the camera object.
	if (m_cam)
	{
		delete m_cam;
		m_cam = 0;
	}

	// Release the D3D object.
	if (m_D3DApp)
	{
		m_D3DApp->Shutdown();
		delete m_D3DApp;
		m_D3DApp = 0;
	}

}

bool Graphics::Render()
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;

	// Clear the buffers to begin the scene.
	m_D3DApp->ClearBuffer(1.0f, 0.0f, 0.0f, 1.0f);

	m_cam->UpdateViewMatrix();

	// Present back buffer to the screen.
	m_D3DApp->PresentBuffer();

	return true;
}

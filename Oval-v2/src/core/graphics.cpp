//---------------------------------- -
//FILE : graphics.cpp
//-----------------------------------

#include "graphics.h"

Graphics::Graphics()
	: m_D3DApp(0),
	  m_model(0),
	  m_voxelization(0)
{
}

Graphics::~Graphics()
{

}

bool Graphics::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//---
	//Create the Direct3D object.
	//---
	m_D3DApp = new D3DApp();
	if (!m_D3DApp)
	{
		return false;
	}

	result = m_D3DApp->Init(screenWidth, screenHeight, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	//---
	//Create the model object.
	//---
	m_model = new model();

	m_model->Init(m_D3DApp->GetDevice(), "data/CornellBox.obj", "data/");

	if (!m_model)
	{
		MessageBox(hwnd, L"Could not load model.", L"Error", MB_OK);
		return false;
	}

	//---
	//Create the voxelization shader object.
	//---
	m_voxelization = new VoxelizationShader();

	m_voxelization->Init(m_D3DApp->GetDevice(), hwnd,L"shader/voxelization.vs.hlsl", L"shader/voxelization.ps.hlsl", L"shader/voxelization.gs.hlsl");

	if (!m_voxelization)
	{
		MessageBox(hwnd, L"Could not load voxelization shader.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Graphics::Update(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj)
{
	bool result;

	result = Render(world, view, proj);
	if (!result)
	{
		return false;
	}

	return true;
}


void Graphics::Shutdown()
{
	// Release the voxeliaztion shader object.
	if (m_voxelization)
	{
		delete m_voxelization;
		m_voxelization = 0;
	}

	// Release the model object.
	if (m_model)
	{
		delete m_model;
		m_model = 0;
	}

	// Release the D3D object.
	if (m_D3DApp)
	{
		m_D3DApp->Shutdown();
		delete m_D3DApp;
		m_D3DApp = 0;
	}

}

bool Graphics::Render(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj)
{

	//clear the buffers to begin the scene.
	m_D3DApp->ClearBuffer(0.0f, 1.0f, 1.0f, 1.0f);

	//update model
	m_model->Render(m_D3DApp->GetDeviceContext());

	//update shaderApp
	m_voxelization->Render(m_D3DApp->GetDeviceContext(), m_model->GetIndexCount(), world, view, proj,NULL);

	//present back buffer to the screen.
	m_D3DApp->PresentBuffer();

	return true;
}


//-----------------------------------
//FILE			: graphics.h
//DESCRIPTION	: Handle graphics of every object
//PARENT		: System class 
//NODE			: d3dApp, camera,model,shader class
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "d3dApp.h"
#include "model.h"
#include "voxelization_shader.h"

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Init(int screenWidth, int screenHeight, HWND hwnd);
	bool Update(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj);
	void Shutdown();

private:
	bool Render(const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj);

private:
	D3DApp *m_D3DApp;
	model  *m_model;
	VoxelizationShader *m_voxelization;
};

#endif // GRAPHICS_H
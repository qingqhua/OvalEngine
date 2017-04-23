//-----------------------------------
//FILE			: graphics.h
//DESCRIPTION	: Handle graphics of every object
//PARENT		: System class 
//NODE			: d3dApp, camera class
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "d3dApp.h"
#include "camera.h"
#include "model.h"

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Init(int screenWidth, int screenHeight, HWND hwnd);
	bool Update();
	void Shutdown();

private:
	bool Render();

private:
	D3DApp *m_D3DApp;

	camera *m_cam;
};

#endif // GRAPHICS_H
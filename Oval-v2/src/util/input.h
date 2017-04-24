//-----------------------------------
//FILE			: input.h
//DESCRIPTION	: use direct-input to handle input event
//PARENT		: systemInst class 
//NODE			: EMPTY
//REFERENCE		: rastertek.com
//-----------------------------------
#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class Input
{
public:
	Input();
	~Input();

	bool Init(HINSTANCE hInstance, HWND hwnd, int width, int height);
	void Shutdown();
	bool Update();

	void GetMouseLocation(int& mouseX, int& mouseY);

	bool IsEscapePressed();

	//W S A D Q E
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsBackPressed();
	bool IsForwardPressed();
	bool IsUpPressed();
	bool IsDownPressed();


private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

};

#endif // INPUT_H
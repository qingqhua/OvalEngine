//-----------------------------------
//FILE : main_gi.cpp
//-----------------------------------

#include "core/systemInst.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	System* mySystem;
	bool result;


	// Create the system object.
	mySystem = new System;
	if (!mySystem)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = mySystem->Init();
	if (result)
	{
		mySystem->Run();
	}

	// Shutdown and release the system object.
	if (!mySystem->Run())
	{
		mySystem->Shutdown();
		delete mySystem;
		mySystem = 0;
	}

	return 0;
}


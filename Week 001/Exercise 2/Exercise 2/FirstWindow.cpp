

#include "WindowApp.h"
 
class InitDirect3DApp : public WindowApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	InitDirect3DApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
: WindowApp(hInstance) 
{
}

InitDirect3DApp::~InitDirect3DApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();
}

void InitDirect3DApp::initApp()
{
	WindowApp::initApp();
}

void InitDirect3DApp::onResize()
{
	WindowApp::onResize();
}

void InitDirect3DApp::updateScene(float dt)
{
	WindowApp::updateScene(dt);
}

void InitDirect3DApp::drawScene()
{
	WindowApp::drawScene();

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	// Exercise 2.2
	R.top = R.top + 50;
	//R.bottom = R.bottom + 100;

	mFont->DrawText(0, mTimeStats.c_str(), -1, &R, DT_NOCLIP, RED);
	
	// Exercise 2.2
	R.top = R.top + 100;
	//R.bottom = R.bottom + 100;

	mFont->DrawText(0, mAdapterStats.c_str(), -1, &R, DT_NOCLIP, BLUE);

	mSwapChain->Present(0, 0);
}

#ifndef WINDOWAPP_H
#define WINDOWAP_H


#include "Utility.h"
#include "Timer.h"
#include <string>
#include <ctime>
#include <vector>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx10d.lib" )
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxguid.lib" )

class WindowApp
{
public:
	WindowApp(HINSTANCE hInstance);
	virtual ~WindowApp();

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	int run();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual void initApp();
	virtual void onResize();// reset projection/etc
	virtual void updateScene(float dt);
	virtual void drawScene(); 
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void ToggleFullScreen();

	void DetectAdapters();

protected:
	void initMainWindow();
	void initDirect3D();
	
protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;

	Timer mTimer;

	std::wstring mFrameStats;
	std::wstring mTimeStats;
	bool m_bFullScreen;
	std::wstring mAdapterStats;


	ID3D10Device*    md3dDevice;
	IDXGISwapChain*  mSwapChain;
	ID3D10Texture2D* mDepthStencilBuffer;
	ID3D10RenderTargetView* mRenderTargetView;
	ID3D10DepthStencilView* mDepthStencilView;
	ID3DX10Font* mFont;


	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D10_DRIVER_TYPE md3dDriverType;
	D3DXCOLOR mClearColor;
	int mClientWidth;
	int mClientHeight;

	std::vector <IDXGIAdapter*> mvAdapters;
	std::vector<IDXGIOutput*> mvOutputs;
	DXGI_MODE_DESC * mpDescs;
	UINT numModes;
};




#endif
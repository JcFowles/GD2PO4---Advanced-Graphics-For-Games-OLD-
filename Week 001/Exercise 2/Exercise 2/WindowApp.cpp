

#include "WindowApp.h"
#include <sstream>

LRESULT CALLBACK
MainWndProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	static WindowApp* app = 0;



	switch (_msg)
	{
		case WM_CREATE:
		{
			// Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
			CREATESTRUCT* cs = (CREATESTRUCT*)_lParam;
			app = (WindowApp*)cs->lpCreateParams;
			return 0;
		}
		case WM_KEYDOWN:
		{
			switch (_wParam)
			{
			case VK_ESCAPE:
			{
				PostQuitMessage(0);
				return 0;
			}
			break;
			case VK_F1:
			{
				app->ToggleFullScreen();
			}
				break;
			}
		}
			break;

	}

	// Don't start processing messages until after WM_CREATE.
	if( app )
		return app->msgProc(_msg, _wParam, _lParam);
	else
		return DefWindowProc(_hwnd, _msg, _wParam, _lParam);
}

WindowApp::WindowApp(HINSTANCE hInstance)
{
	mhAppInst   = hInstance;
	mhMainWnd   = 0;
	mAppPaused  = false;
	mMinimized  = false;
	mMaximized  = false;
	mResizing   = false;

	mFrameStats = L"";
	mTimeStats = L"";
	m_bFullScreen = false;
 
	md3dDevice          = 0;
	mSwapChain          = 0;
	mDepthStencilBuffer = 0;
	mRenderTargetView   = 0;
	mDepthStencilView   = 0;
	mFont               = 0;

	mMainWndCaption = L"D3D10 Application";
	md3dDriverType  = D3D10_DRIVER_TYPE_HARDWARE;

	// Exercise 2.2 
	mClearColor = ORANGE;
	
	mClientWidth    = 800;
	mClientHeight   = 600;
}

WindowApp::~WindowApp()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(md3dDevice);
	ReleaseCOM(mFont);

	delete mpDescs;
}
	

HINSTANCE WindowApp::getAppInst()
{
	return mhAppInst;
}

HWND WindowApp::getMainWnd()
{
	return mhMainWnd;
}

int WindowApp::run()
{
	MSG msg = {0};
 
	mTimer.Reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			mTimer.Tick();

			if( !mAppPaused )
				updateScene(mTimer.GetDeltaTime());	
			else
				Sleep(50);

			drawScene();
        }
    }
	return (int)msg.wParam;
}

void WindowApp::initApp()
{
	initMainWindow();
	initDirect3D();

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 24;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy_s(fontDesc.FaceName, L"Times New Roman");

	D3DX10CreateFontIndirect(md3dDevice, &fontDesc, &mFont);
}
 
void WindowApp::onResize()
{
	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D10Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);


	// Create the depth/stencil buffer and view.

	D3D10_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = mClientWidth;
	depthStencilDesc.Height    = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; // multisampling must match
	depthStencilDesc.SampleDesc.Quality = 0; // swap chain values.
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	md3dDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	

	// Set the viewport transform.

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = mClientWidth;
	vp.Height   = mClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	md3dDevice->RSSetViewports(1, &vp);
}

void WindowApp::updateScene(float dt)
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.

	static int frameCnt = 0;
	static float t_base = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (mTimer.GetGameTime() - t_base) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << L"FPS: " << fps << L"\n" 
			 << "Milliseconds: Per Frame: " << mspf;
		mFrameStats = outs.str();
		
		//Clear the String buffer
		outs.str(L"");

		// Get the system and local  Date and time 
		SYSTEMTIME SystemTime, LocalTime;
		//To get the local time
		GetLocalTime(&LocalTime);
		//To get the system time
		GetSystemTime(&SystemTime);
	
		outs << L"Local Time: " << LocalTime.wHour << L":" << LocalTime.wMinute << L":" << LocalTime.wSecond << L"\n"
			<< "Local Date: " << LocalTime.wDay << L"/" << LocalTime.wMonth << L"/" << LocalTime.wYear << L"\n"
			<< L"System Time: " << SystemTime.wHour << L":" << SystemTime.wMinute << L":" << SystemTime.wSecond << L"\n"
			<< "Local Date: " << SystemTime.wDay << L"/" << SystemTime.wMonth << L"/" << SystemTime.wYear;
		mTimeStats = outs.str();
		
		//Clear the String buffer
		outs.str(L"");
		
		//Number of adapters
		outs << L"*** NUM ADAPTERS = " << mvAdapters.size() << L"\n";
		std::string strSupportedAdapters = "*** D3D10 SUPPORTED FOR ADAPTER: ";
		std::string NumOfOutputs = "*** NUM OUTPUTS FOR DEFAULT ADAPTER = ";
		for (unsigned int i = 0; i < mvAdapters.size(); i++)
		{
			//outs << L"*** Adapter: " << i << L" : ";
			if ((mvAdapters[i]->CheckInterfaceSupport(__uuidof(ID3D10Device), 0) == S_OK))
			{
				strSupportedAdapters += std::to_string(i) + ",";
			}
			NumOfOutputs += std::to_string(mvOutputs.size());
						
		}

		outs << strSupportedAdapters.c_str() << L"\n";
		outs << NumOfOutputs.c_str() << L"\n";

		for (UINT i = 0; i < numModes; i++)
		{
			outs << L"Width : " << (&mpDescs[i])->Width << L" | Height: " << (&mpDescs[i])->Height << L" | Refresh: " << ((float)(&mpDescs[i])->RefreshRate.Numerator / (float)(&mpDescs[i])->RefreshRate.Denominator) << L"\n";// << (&mpDescs[i])->RefreshRate.Denominator << L"\n";
		}

		mAdapterStats = outs.str();
		


		// Reset for next average.
		frameCnt = 0;
		t_base  += 1.0f;
	}



}

void WindowApp::drawScene()
{
	md3dDevice->ClearRenderTargetView(mRenderTargetView, mClearColor);
	md3dDevice->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

LRESULT WindowApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth  = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if( md3dDevice )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				onResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( mMinimized )
				{
					mAppPaused = false;
					mMinimized = false;
					onResize();
				}

				// Restoring from maximized state?
				else if( mMaximized )
				{
					mAppPaused = false;
					mMaximized = false;
					onResize();
				}
				else if( mResizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					onResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing  = true;
		mTimer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing  = false;
		mTimer.Start();
		onResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;
	}

	return DefWindowProc(mhMainWnd, msg, wParam, lParam);
}

void WindowApp::initMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, this); 
	if( !mhMainWnd )
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
}

void WindowApp::initDirect3D()
{
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// No multisampling.
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;


	// Create the device.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	HR( D3D10CreateDeviceAndSwapChain(
			0,                 // default adapter
			md3dDriverType,
			0,                 // no software device
			createDeviceFlags, 
			D3D10_SDK_VERSION,
			&sd,
			&mSwapChain,
			&md3dDevice) );


	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the onResize method here to avoid code duplication.
	
	onResize();

	//Exercise 2.3
	DetectAdapters();
}

//Exercise 2.3
void WindowApp::ToggleFullScreen()
{
	m_bFullScreen = !m_bFullScreen;
	mSwapChain->SetFullscreenState(m_bFullScreen, 0);
}

//Exercise 2.3
void WindowApp::DetectAdapters()
{
	IDXGIFactory * pFactory = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

	UINT uiAdapter = 0;
	UINT uiOutput = 0;
	IDXGIAdapter * pAdapter;
	IDXGIOutput * pOutput;
	while (pFactory->EnumAdapters(uiAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		mvAdapters.push_back(pAdapter);
		++uiAdapter;

		while (pAdapter->EnumOutputs(uiOutput, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			mvOutputs.push_back(pOutput);
			++uiOutput;
		}

	}

	numModes = 0;
	UINT Num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT flags = DXGI_ENUM_MODES_INTERLACED;
	mpDescs = NULL;
	if (mvOutputs.size() > 0)
	{
		mvOutputs[0]->GetDisplayModeList(format, flags, &numModes, 0);
	
		mpDescs = new DXGI_MODE_DESC[numModes];
		mvOutputs[0]->GetDisplayModeList(format, flags, &numModes, mpDescs);
	}
	
	

	ReleaseCOM(pFactory);

}
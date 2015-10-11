/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Application.cpp
* Description : Main Application functionality
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

// This Include
#include "Application.h"

// *** Windows Functions ***

// Static Variables
CApplication* CApplication::s_pApp = 0;

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _CmdShow)
{
	//Seed the random generator
	srand((unsigned int)time(NULL));

	// Create the Application 
	CApplication* pApp = CApplication::GetInstance();
	
	// Create a window for the application based on the width/height and Title given
	// Check if Created successfully 
	if (pApp->CreateAndRegisterWindow(_hInstance, 1000, 1000, L"Exercise 001" ) )
	{
		VALIDATE(pApp->Initialise());
		pApp->RunApp();
	}

	// Delete all contents of the Application
	pApp->DestroyInstance();
	return 0;
}

LRESULT CALLBACK CApplication::WindowProc(HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	// Retrieve the Instance of the Application
	CApplication* pApp = CApplication::GetInstance();

	// Switch case dependent on the message sent
	switch (_uiMsg)
	{
	case WM_DESTROY:	
	// Fall Through
	case WM_CLOSE:
	{
		// Kill the application, this sends a WM_QUIT message.
		PostQuitMessage(0);
	}
		break;
	case WM_KEYDOWN:
	{
		// Only accepts the input once per key press
		if (((_lParam >> 30) & 1) != 1)
		{
			//Set key press to for this key to true
			pApp->SetKeyPress(_wParam, true);
		}
	}
		break;
	case WM_KEYUP:
	{
		//Set key press to for this key to false
		pApp->SetKeyPress(_wParam, false);
	}
		break;
	default: break;
	} 
	// End switch

	// Process any messages left to process
	return (DefWindowProc(_hWnd, _uiMsg, _wParam, _lParam));
}

bool CApplication::CreateAndRegisterWindow(HINSTANCE _hInstance, int _width, int _height, LPCWSTR _lpTitle)
{
	WNDCLASSEX winClass;

	// Save the Windows dimensions
	m_windowWidth = _width;
	m_windowHeight = _height;
	
	// Clear out the window class for use
	ZeroMemory(&winClass, sizeof(WNDCLASSEX));

	//Fill in the struct with the needed information
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = _hInstance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)BLACK_BRUSH;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = WINDOWS_CLASS_NAME;
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Register the window class and check if successful
	VALIDATE(RegisterClassEx(&winClass));
	
	// Create the window and return the result as the handle
	m_hWnd = CreateWindowEx(NULL,												// Extend style.
							WINDOWS_CLASS_NAME,									// Class.		
							_lpTitle,											// Title.
							WS_OVERLAPPEDWINDOW | WS_BORDER |
							WS_CAPTION | WS_SYSMENU | WS_VISIBLE,				// Window style(Windowed)
							CW_USEDEFAULT, CW_USEDEFAULT,						// Initial x,y.
							m_windowWidth, m_windowHeight,						// Initial width, height.
							NULL,												// Handle to parent.
							NULL,												// Handle to menu.
							_hInstance,											// Instance of this application.
							NULL);												// Extra creation parameters.

	// Check if window was created successfully
	VALIDATE(m_hWnd);

	// Return the Successful Creation
	return true;
}

int CApplication::RunApp()
{
	//This holds Windows event messages
	MSG uiMsg;

	//Clear out the event message for use
	ZeroMemory(&uiMsg, sizeof(MSG));

	// Enter main event loop.
	while (uiMsg.message != WM_QUIT)
	{
		while (PeekMessage(&uiMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&uiMsg);
			DispatchMessage(&uiMsg);
		}

		// Execute one frame of application
		//ExecuteOneFrame(); 

		if (ExecuteOneFrame())
		{
			//ShutDown();
			break;
		}
	}

	// Return this part of the WM_QUIT message to Windows
	return (static_cast<int>(uiMsg.wParam));
}

// *** Application Functions ***

// Creation and Deletion

CApplication::CApplication()
{
}

CApplication::~CApplication()
{
}

CApplication* CApplication::GetInstance()
{
	// If the application doesnt exist create it
	if (s_pApp == 0)
	{
		s_pApp = new CApplication();
	}
	// Return a pointer to the Application
	return s_pApp;
}

bool CApplication::Initialise()
{

	// Create a Renderer for graphics
	m_pRenderManager = new CDX10Renderer();
	VALIDATE(m_pRenderManager->Initialise(m_windowWidth, m_windowHeight, m_hWnd));

	// Initialise boolean array for KeyDown to false
	m_pKeyPress = new bool[255];
	memset(m_pKeyPress, false, 255);
	
	// Set game exit false;
	m_exit = false;

	// Create a Mesh

	// Cube Mesh
	m_pCubeMesh = new CMesh_Cube();
	m_pCubeMesh->Initialise(m_pRenderManager, Color::RED);
	
	// Grid Mesh
	m_pGridMesh = new CMesh_Grid();
	m_pGridMesh->Initialise(m_pRenderManager, 150, 150, Color::RED);

	// Create Cube objects Using the Cube mesh
	m_pCube = new C3DObject();
	VALIDATE(m_pCube->Initialise(m_pRenderManager, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_pCubeMesh,Color::BLUE, float3(3.0f, 3.0f, 3.0f)));
	VALIDATE((m_pCube->BuildEffect("color.fx", "ColorTechnique")));
	m_pCube->SetPos(float3(0.0f, 0.0f,0.0f));

	// Create Grid objects Using the Grid mesh
	m_pGrid = new C3DObject();
	VALIDATE(m_pGrid->Initialise(m_pRenderManager, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, m_pGridMesh, Color::RED, float3(1.0f, 1.0f, 1.0f)));
	VALIDATE(m_pGrid->BuildEffect("color.fx", "ColorChangeTechnique"));
	m_pGrid->SetPos(float3(0.0f, -12.0f, 0.0f));
	
	// Create the Debug camera
	m_pFreeCamera = new CDebugCamera();
	VALIDATE(m_pFreeCamera->Initialise(m_pRenderManager));

	// Create the Timer
	m_clock = new Timer();

	// Return the Successful Initialization 
	return true;
}

void CApplication::ShutDown()
{
	ReleasePointer(m_pFreeCamera);
	ReleasePointer(m_pCube);
	ReleasePointer(m_pGrid);
	ReleasePointer(m_pKeyPress);
	ReleasePointer(m_clock);

	ReleasePointer(m_pCubeMesh);
	ReleasePointer(m_pGridMesh);

	m_pRenderManager->ShutDown();
	ReleasePointer(m_pRenderManager);
}

void CApplication::DestroyInstance()
{
	s_pApp->ShutDown();
	ReleasePointer(s_pApp);
}

// Application Functionality

bool CApplication::ExecuteOneFrame()
{

	m_clock->Tick();

	ProcessInput(m_clock->GetDeltaTime());
		
	m_pRenderManager->StartRender();
	
	m_pFreeCamera->Process();
	m_pRenderManager->CalcViewMatrix(m_pFreeCamera->GetPosition(), m_pFreeCamera->GetTarget(), m_pFreeCamera->GetUp());

	m_pCube->Process(m_clock->GetDeltaTime());
	m_pCube->Draw();
	
	time += m_clock->GetDeltaTime();
	
	// Reset Time every second
	if (time > 1.0f)
	{
		time = 0.0f;
	}

	m_pGrid->Process(time);
	m_pGrid->Draw();

	m_pRenderManager->EndRender();	

	// TO DO: Limit frames
	// Simulates the frame limiter
	Sleep(16);

	return m_exit;
}

// Input Handler

void CApplication::ProcessInput(float _dt)
{
	// Template Inputs
	if (m_pKeyPress[VK_F1])
	{

		m_pRenderManager->ToggleFullscreen();
		m_pKeyPress[VK_F1] = false;
	}

	if (m_pKeyPress[VK_F2])
	{

		m_pRenderManager->ToggleWireFrame();
		m_pKeyPress[VK_F2] = false;
	}

	if ((m_pKeyPress[VK_NUMPAD4]) && !(m_pKeyPress[VK_NUMPAD6]))
	{
		m_pFreeCamera->Strafe(-1 * _dt);
	}

	if ((m_pKeyPress[VK_NUMPAD6]) && !(m_pKeyPress[VK_NUMPAD4]))
	{
		m_pFreeCamera->Strafe( _dt);
	}

	if ((m_pKeyPress[VK_NUMPAD8]) && !(m_pKeyPress[VK_NUMPAD2]))
	{
		m_pFreeCamera->Move(_dt);
	}

	if ((m_pKeyPress[VK_NUMPAD2]) && !(m_pKeyPress[VK_NUMPAD8]))
	{
		m_pFreeCamera->Move(-1 * _dt);
	}

	if ((m_pKeyPress[VK_NUMPAD1]) && !(m_pKeyPress[VK_NUMPAD0]))
	{
		m_pFreeCamera->Fly(_dt);
	}

	if ((m_pKeyPress[VK_NUMPAD0]) && !(m_pKeyPress[VK_NUMPAD1]))
	{
		m_pFreeCamera->Fly(-1 * _dt);
	}
	
	if ((m_pKeyPress[VK_UP]) && !(m_pKeyPress[VK_DOWN]))
	{
		m_pFreeCamera->Pitch(-1*_dt);
	}

	if ((m_pKeyPress[VK_DOWN]) && !(m_pKeyPress[VK_UP]))
	{
		m_pFreeCamera->Pitch(_dt);
	}

	if ((m_pKeyPress[VK_LEFT]) && !(m_pKeyPress[VK_RIGHT]))
	{
		m_pFreeCamera->Yaw(-1 * _dt);
	}

	if ((m_pKeyPress[VK_RIGHT]) && !(m_pKeyPress[VK_LEFT]))
	{
		m_pFreeCamera->Yaw(_dt);
	}

	if ((m_pKeyPress[VK_ESCAPE]))
	{
		if (m_pRenderManager->GetFullScreenMode() == true)
		{
			m_pRenderManager->ToggleFullscreen();
		}
		m_exit = true;
	}

	// Application Specific Inputs
}

void CApplication::SetKeyPress(int _keyIndex, bool _press)
{
	m_pKeyPress[_keyIndex] = _press;
}


#include "DX10Renderer.h"

// Creation and Deletion

CDX10Renderer::CDX10Renderer()
{
	// Window Variables
	m_hWnd = 0;
	m_fullScreen = false;

	// DX10 Variables
	m_pDX10Device = 0;
	m_pDX10SwapChain = 0;
	m_pRenderTargetView = 0;

}

CDX10Renderer::~CDX10Renderer()
{
}

bool CDX10Renderer::Initialise(int _windowWidth, int _windowHeight, HWND _hWND)
{
	// Save Windows Variables
	m_hWnd = _hWND;
	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

	// Create the Device and Swap chain 
	VALIDATE(InitialiseDeviceAndSwapChain());

	// Set Clear Color
	m_clearColor = MAGENTA;

	// Return the Succesful Initialization 
	return true;
}

void CDX10Renderer::ShutDown()
{
	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pDX10SwapChain);
	ReleaseCOM(m_pDX10Device);
}

// Renderer Functionality

bool CDX10Renderer::InitialiseDeviceAndSwapChain(){}

void CDX10Renderer::onResize(){}

void CDX10Renderer::RenderFrame(){}
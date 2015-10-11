/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : DX10Renderer.cpp
* Description : DirectX10 Render manager that is used to Render all Graphical components
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

// This Included
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
	m_wireFrame = false;

	// ID Keys for the Maps
	nxtEffectID = 0;
	nxtTechID = 0;
	nxtInputLayoutID = 0;

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
	m_clearColor = Color::PINK;
	
	// Return the Successful Initialization 
	return true;
}

void CDX10Renderer::ShutDown()
{

	// Release the DX10 InputLayers  
	std::map<UINT, ID3D10InputLayout*>::iterator iterInputLayout = m_inputLayouts.begin();
	while (iterInputLayout != m_inputLayouts.end())
	{
		ReleaseCOM(iterInputLayout->second);
		iterInputLayout++;
	}

	// Release the DX10 Effects
	std::map<UINT, ID3D10Effect*>::iterator iterFX = m_effects.begin();
	while (iterFX != m_effects.end())
	{
		ReleaseCOM(iterFX->second);
		iterFX++;
	}

	// Release the static Buffers
	std::map<UINT, CStaticBuffer*>::iterator iterBuffers = m_staticBuffers.begin();
	while (iterBuffers != m_staticBuffers.end())
	{
		ReleasePointer(iterBuffers->second);
		iterBuffers++;
	}


	ReleaseCOM(m_pRasterizerStates);
	ReleaseCOM(m_pDepthStencilBuffer);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pDX10SwapChain);

	if (m_pDX10Device != 0)
	{
		// Reset the Device States to Default
		m_pDX10Device->ClearState();
	}
	ReleaseCOM(m_pDX10Device);
}

// Creation of required

bool CDX10Renderer::InitialiseDeviceAndSwapChain()
{
	// Set Driver Type to Hardware (HAL)
	m_DX10DriverType = D3D10_DRIVER_TYPE_HARDWARE;

	// Fill out the DXGI Swap Chain parameters
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferDesc.Width = m_windowWidth;
	swapDesc.BufferDesc.Height = m_windowHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;// DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// DXGI_MODE_SCALING_CENTERED;// DXGI_MODE_SCALING_UNSPECIFIED;

	swapDesc.SampleDesc.Count = 1;		// Set No multisampling per pixel ( 1 sample only)
	swapDesc.SampleDesc.Quality = 0;	// Set to low Quality

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//0;

	// Add Debug Info to the flags when run in Debug mode
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	// Create the Device and Swap Chain and Validate it
	VALIDATE_HRESULT(D3D10CreateDeviceAndSwapChain(	0,                 // Use default adapter
													m_DX10DriverType,
													0,                 // no software device
													createDeviceFlags,
													D3D10_SDK_VERSION,
													&swapDesc,
													&m_pDX10SwapChain,
													&m_pDX10Device));
	
	// Run onResize For initial creation of window 
	VALIDATE(onResize());
	
	// Return the Successful Initialization 
	return true;
}

bool CDX10Renderer::BuildEffect(std::string _fxFileName, std::string _technique, UINT* _pFXID, UINT* _pTechID)
{
	ID3D10Effect* pDX10FX = 0;
	UINT fxID;
	UINT techID;

	// *** Effect Map ***

	// Add the Effect File to the Map of Effects
	std::map<std::string, UINT>::iterator fxCheckIter;
	fxCheckIter = m_effectIDs.find(_fxFileName);

	// Check if its already been added if not add it
	if (fxCheckIter != m_effectIDs.end())
	{
		// If its already in the Map of Effects use the found Effect's reference IDs
		fxID = fxCheckIter->second;
		pDX10FX = m_effects.find(fxID)->second;
	}
	else
	{
		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		// Create the effect from the .fx files
		ID3D10Blob* compilationErrors;
		// File Path to the .fx file
		std::string fxFilePath = "Assets/FX/" + _fxFileName;
		VALIDATE_HRESULT(D3DX10CreateEffectFromFileA(	fxFilePath.c_str(), 0, 0,
														"fx_4_0", shaderFlags, 0,
														m_pDX10Device, 0, 0, &pDX10FX,
														&compilationErrors, 0));
		ReleaseCOM(compilationErrors);

		// Add the effect to the Map of Effects and the map of effect IDs
		fxID = ++nxtEffectID;
		std::pair<std::string, UINT> fxPair(_fxFileName, fxID);
		std::pair<UINT, ID3D10Effect*> fxPairByID(fxID, pDX10FX);

		VALIDATE(m_effectIDs.insert(fxPair).second);
		VALIDATE(m_effects.insert(fxPairByID).second);
	}

	// *** Technique Map ***

	// Adding the Technique to the Map of Techniques
	std::map<UINT, std::map<std::string, UINT>>::iterator fxIDCheckIter;
	fxIDCheckIter = m_techIDs.find(fxID);

	// Check if Technique Map of IDs for the effect ID 
	if (fxIDCheckIter != m_techIDs.end())
	{
		// If Found 
				
		std::map<std::string, UINT>::iterator techIDCheckIter;
		techIDCheckIter = fxIDCheckIter->second.find(_technique);

		// Check The inner map of Technique IDs to see if the Technique it self has been Added 
		if (techIDCheckIter != fxIDCheckIter->second.end())
		{
			// If Found use the found Tech ID
			techID = fxCheckIter->second;
		}
		else
		{
			// If not Found  - Create and Add it to the inner Map of Technique IDs
			ID3D10EffectTechnique* pTech = pDX10FX->GetTechniqueByName(_technique.c_str());

			techID = ++nxtTechID;
			std::pair<std::string, UINT> techPair(_technique, techID);
			std::pair<UINT, ID3D10EffectTechnique*> techPairByID(techID, pTech);

			VALIDATE((&fxIDCheckIter->second)->insert(techPair).second);
			VALIDATE(m_techniques.insert(techPairByID).second);
		}
	}
	else
	{
		// If effect ID not Found - Create and Add it to the Outer map for Effects to Techniques

		ID3D10EffectTechnique* pTech = pDX10FX->GetTechniqueByName(_technique.c_str());

		techID = ++nxtTechID;
		std::map<std::string, UINT> innerTechMap;
		std::pair<std::string, UINT> innerTechPair(_technique, techID);
		// Add to Inner Map
		VALIDATE(innerTechMap.insert(innerTechPair).second);

		std::pair<UINT, std::map<std::string, UINT>> outerTechMap(fxID, innerTechMap);
		// Add inner Map to Outer Map
		VALIDATE(m_techIDs.insert(outerTechMap).second);

		std::pair<UINT, ID3D10EffectTechnique*> techByIDPair(techID, pTech);
		// Add the Technique to its own map
		VALIDATE(m_techniques.insert(techByIDPair).second);
	}

	// Store the effect and Technique IDs to the memory locations passed in by the Object.
	*_pFXID = fxID;
	*_pTechID = techID;

	// Return Successful Build
	return true;
}

bool CDX10Renderer::CreateVertexLayout(eVertexType _vertType, UINT _techID, UINT* _pVertexLayoutID)
{
	// Number of Elements in the vertex description
	UINT elementCount = 0;

	switch (_vertType)
	{
		case VT_BASIC: // Vertex with Position
		{
			D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
			};
			elementCount = 1;
			return (CreateVertexLayout(vertexDesc, elementCount, _techID, _pVertexLayoutID));
		}
			break;
		case VT_COLOR: // Vertex with Position and Color
		{
			D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
			};
			elementCount = 2;
			return (CreateVertexLayout(vertexDesc, elementCount, _techID, _pVertexLayoutID));
		}
			break;
		default:
		{
			// Return a failure
			return false;
		}
			break;
	}	
}

bool CDX10Renderer::CreateVertexLayout(D3D10_INPUT_ELEMENT_DESC _vertexDesc[], UINT _elementCount, UINT _techID, UINT* _pVertexLayoutID)
{
	// Find the Technique using the passed in Tech ID
	ID3D10EffectTechnique* pTech = m_techniques.find(_techID)->second;
	ID3D10InputLayout* pVertexLayout;

	D3D10_PASS_DESC passDesc;
	pTech->GetPassByIndex(0)->GetDesc(&passDesc);
	// Create the input layout
	VALIDATE_HRESULT(m_pDX10Device->CreateInputLayout(	_vertexDesc, _elementCount, 
														passDesc.pIAInputSignature,
														passDesc.IAInputSignatureSize,
														&pVertexLayout));

	UINT inputLayerID = ++nxtInputLayoutID;
	std::pair<UINT, ID3D10InputLayout*> inputLayerPair(inputLayerID, pVertexLayout);
	// Add the inputLayout to the Map
	VALIDATE(m_inputLayouts.insert(inputLayerPair).second);
	
	// Store the Vertex Layout ID to the memory locations passed in
	*_pVertexLayoutID = inputLayerID;
	return true;
}

void CDX10Renderer::CalcViewMatrix(float3 _pos, float3 _target, float3 _up)
{
	D3DXVECTOR3 pos;
	pos.x = _pos.x; 
	pos.y = _pos.y;
	pos.z = _pos.z;
	D3DXVECTOR3 target;
	target.x = _target.x;
	target.y = _target.y;
	target.z = _target.z;
	D3DXVECTOR3 up;
	up.x = _up.x;
	up.y = _up.y;
	up.z = _up.z;

	// Set the View Matrix
	D3DXMatrixLookAtLH(&m_matView, &pos, &target, &up);
}

void CDX10Renderer::CalcProjMatrix(float _fFov, float _fNear, float _fFar)
{
	//Calculate the Aspect Ratio
	float aspect = float(m_windowWidth) / m_windowHeight;
	//D3DXMatrixPerspectiveFovLH(&m_matProj, 0.25f*kPI, aspect, 1.0f, 1000.0f);
	D3DXMatrixPerspectiveFovLH(&m_matProj, _fFov, aspect, _fNear, _fFar);
}

// Getter and Setters

ID3D10EffectVariable* CDX10Renderer::GetEffectVariable(UINT _fxID, std::string _techVarName)
{
	// Find the effect by the passed in ID
	ID3D10Effect* pFX = m_effects.find(_fxID)->second;

	// Return the effect variable pointer
	return pFX->GetVariableByName(_techVarName.c_str());
}


ID3D10EffectTechnique* CDX10Renderer::GetTechnique(UINT _techID)
{
	
	std::map<UINT, ID3D10EffectTechnique*>::iterator iterTech = m_techniques.find(_techID);
	if (iterTech == m_techniques.end())
	{
		// Return NULL if technique doesnt exist
		return NULL;
	}

	// Return the Technique
	return iterTech->second;
}

bool CDX10Renderer::SetInputLayout(UINT _vertexLayoutID)
{
	// Find the Vertex Layout in the Map 
	std::map<UINT, ID3D10InputLayout*>::iterator iterVLayout = m_inputLayouts.find(_vertexLayoutID);
	if (iterVLayout == m_inputLayouts.end())
	{
		// Return false if Vertex Layout was not found
		return false;
	}
	ID3D10InputLayout* pVertexLayout = iterVLayout->second;

	// Set the new Vertex layout on the device
	m_pDX10Device->IASetInputLayout(pVertexLayout);

	// Return Successful Set
	return true;
}


// Renderer Functionality

bool CDX10Renderer::onResize()
{
	// Release the Render Target View  so it can be recreated 
	ReleaseCOM(m_pDepthStencilBuffer);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pRenderTargetView);

	// Resize the buffers on the Swap Chain and create the new render target view
	VALIDATE_HRESULT(m_pDX10SwapChain->ResizeBuffers(1, m_windowWidth, m_windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Create a temporary Back buffer
	ID3D10Texture2D* pTempBackBuffer;

	// Retrieve the Temp Back Buffer of the Swap Chain and create a Render Target View using the Temp Back Buffer
	VALIDATE_HRESULT(m_pDX10SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&pTempBackBuffer)));
	VALIDATE_HRESULT(m_pDX10Device->CreateRenderTargetView(pTempBackBuffer, 0, &m_pRenderTargetView));

	// Release the temporary Back Buffer
	ReleaseCOM(pTempBackBuffer);

	// Create the Depth Stencil View to be added to the Target View
	D3D10_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_windowWidth;
	depthStencilDesc.Height = m_windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_pDX10Device->CreateTexture2D(&depthStencilDesc, NULL, &m_pDepthStencilBuffer);
	m_pDX10Device->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView);

	// Bind the Render Target View and the depth-stencil buffer to the output-merger stage of the pipeline
	m_pDX10Device->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// Set the View Port for the Device
	D3D10_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = m_windowWidth;
	viewPort.Height = m_windowHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	// Binds the View port to the Rasterizer stage of the pipeline
	m_pDX10Device->RSSetViewports(1, &viewPort);

	// Set Rasterizer Description
	m_rasterizerState.CullMode = D3D10_CULL_BACK;
	m_rasterizerState.FillMode = D3D10_FILL_SOLID;
	m_rasterizerState.FrontCounterClockwise = true;
	m_rasterizerState.DepthBias = false;
	m_rasterizerState.DepthBiasClamp = 0;
	m_rasterizerState.SlopeScaledDepthBias = 0;
	m_rasterizerState.DepthClipEnable = true;
	m_rasterizerState.ScissorEnable = false;
	m_rasterizerState.MultisampleEnable = false;
	m_rasterizerState.AntialiasedLineEnable = true;
	
	// Set the Rasterizer state to the device
	m_pDX10Device->CreateRasterizerState(&m_rasterizerState, &m_pRasterizerStates);
	m_pDX10Device->RSSetState(m_pRasterizerStates);


	// Calculate the new Projection Matrix for the new screen sizes
	// TO DO:
	CalcProjMatrix(0.25f*kPI, 1.0f, 1000.0f);

	// Return Successful resize
	return true;
}

void CDX10Renderer::RestoreDefaultDrawStates()
{
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDX10Device->OMSetDepthStencilState(0, 0);
	m_pDX10Device->OMSetBlendState(0, blendFactors, 0xFFFFFFFF);	
}

void CDX10Renderer::SetTopology(D3D_PRIMITIVE_TOPOLOGY _topology)
{
	m_pDX10Device->IASetPrimitiveTopology(_topology);
}

void CDX10Renderer::ToggleFullscreen()
{
	m_fullScreen = !m_fullScreen;
	m_pDX10SwapChain->SetFullscreenState(m_fullScreen, NULL);
	//onResize();
}

void CDX10Renderer::ToggleWireFrame()
{
	m_wireFrame = !m_wireFrame;

	if (m_wireFrame)
	{
		m_rasterizerState.FillMode = D3D10_FILL_WIREFRAME;
	}
	else
	{
		m_rasterizerState.FillMode = D3D10_FILL_SOLID;
	}


	m_pDX10Device->CreateRasterizerState(&m_rasterizerState, &m_pRasterizerStates);
	m_pDX10Device->RSSetState(m_pRasterizerStates);


}

//Rendering

bool CDX10Renderer::RenderObject(UINT _staticBufferID, UINT _vertexLayoutID, UINT _techniqueID)
{

	// Restore Draw states
	RestoreDefaultDrawStates();
	SetInputLayout(_vertexLayoutID);

	// Get this objects technique 
	ID3D10EffectTechnique* pTech = GetTechnique(_techniqueID);

	D3D10_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	
	std::map<UINT, CStaticBuffer*>::iterator iterBuffer = m_staticBuffers.find(_staticBufferID);
	if (iterBuffer == m_staticBuffers.end())
	{
		return false;
	}
	
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{

		// Apply the Pass 
		pTech->GetPassByIndex(p)->Apply(0);

		// Render the found buffer
		iterBuffer->second->Render();
	}
	
	return true;
}

void CDX10Renderer::PresentClearScreen()
{
	StartRender();
	EndRender();
}

void CDX10Renderer::StartRender()
{
	// Color the entire Back Buffer (the Render Target View) with the Renderer s clear color
	m_pDX10Device->ClearRenderTargetView(m_pRenderTargetView, m_clearColor);
	// Clear the Depth Stencil
	m_pDX10Device->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void CDX10Renderer::EndRender()
{
	// Present the Back Buffer with no synchronization
	m_pDX10SwapChain->Present(0, 0);
}


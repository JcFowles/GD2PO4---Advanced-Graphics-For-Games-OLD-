/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : DX10Renderer.h
* Description : DirectX10 Render manager that is used to Render all Graphical components
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __DX10RENDERER_H__
#define __DX10RENDERER_H__

// Library Includes

// Local Includes
#include "DX10Utilities.h"
#include "Vertex.h"
#include "StaticBuffer.h"

class CDX10Renderer
{
	// Member Functions
public:

	// Creation and Deletion

	/***********************
	* CDX10Renderer: Default Constructor for the DX10Renderer 
	* @author: Jc Fowles
	* @return: 
	********************/
	CDX10Renderer();

	/***********************
	* ~CDX10Renderer: Default Destructor for the DX10Renderer
	* @author: Jc Fowles
	* @return:
	********************/
	~CDX10Renderer();

	/***********************
	* Initialise: Initialise the DX10Renderer
	* @author: Jc Fowles
	* @Parameter: _windowWidth: Width of the application window
	* @Parameter: _windowHeight: Height of the application window
	* @Parameter: _hWND: Handle to the Application window
	* @return: bool: Successful Initialization
	********************/
	bool Initialise(int _windowWidth, int _windowHeight, HWND _hWND);

	/***********************
	* ShutDown: Shuts down the DX10Renderer and releases all memory Created by it.
	* @author: Jc Fowles
	* @return: void
	********************/
	void ShutDown();

	// Renderer Functionality

	/***********************
	* InitialiseDeviceAndSwapChain: Initialise the DX10 Device and Swap Chain
	* @author: Jc Fowles
	* @return: bool: Successful Initialization
	********************/
	bool InitialiseDeviceAndSwapChain();

	/***********************
	* onResize: Recreate Depth and Render Views when resizing the application
	* @author: Jc Fowles
	* @return: bool: Successful Resize
	********************/
	bool onResize();
	
	/***********************
	* ToggleFullscreen: Toggle Full screen mode
	* @author: Jc Fowles
	* @return: void
	********************/
	void ToggleFullscreen();
		
	/***********************
	* ToggleWireFrame: Toggles the wire frame mode
	* @author: Jc Fowles
	* @return: void: 
	********************/
	void ToggleWireFrame();

	/***********************
	* BuildEffect: Build the effects and Techniques from the .fx file
	* @author: Jc Fowles
	* @Parameter: _fxFileName: Name of the .fx file to build from
	* @Parameter: _technique: Name of the Technique from the .fx file to build from
	* @Parameter: _pFXID: Pointer to the Reference ID to the built effect 
	* @Parameter: _pTechID: Pointer to the Reference ID to the built Technique 
	* @return: bool: Successful Build/Creation
	********************/
	bool BuildEffect(std::string _fxFileName, std::string _technique, UINT* _pFXID, UINT* _pTechID);

	/***********************
	* GetEffectVariable: Returns the Effect Variable 
	* @author: Jc Fowles
	* @Parameter: _fxID: Reference ID of the effect to access variable from
	* @Parameter: _techVarName: Name of the variable to retrieve
	* @return: ID3D10EffectVariable*: The Effect Variable
	********************/
	ID3D10EffectVariable* GetEffectVariable(UINT _fxID, std::string _techVarName);

	/***********************
	* CreateVertexDesc: Create and Initialise the Vertex Description
	* @author: Jc Fowles
	* @Parameter: _vertType: Vertex structure to base layout on 
	* @Parameter: _techID: Reference ID of the Technique to base the layout on
	* @Parameter: _pVertexLayoutID: Pointer to the Reference ID to the the created Vertex Layout
	* @return: bool: Successful creation
	********************/
	bool CreateVertexLayout(eVertexType _vertType, UINT _techID, UINT* _pVertexLayoutID);

	/***********************
	* CreateVertexLayout: Create the Vertex Layout for an Object
	* @author: Jc Fowles
	* @parameter: _vertexDesc: Array of Vertex Descriptions
	* @parameter: _elementCount: Number of elements in the Array of Vertex Descriptions
	* @parameter: _techID: Reference ID of the Technique to base the layout on
	* @Parameter: _pVertexLayoutID: Pointer to the Reference ID to the created Vertex Layout
	* @return: bool: Successful Creation
	********************/
	bool CreateVertexLayout(D3D10_INPUT_ELEMENT_DESC _vertexDesc[], UINT _elementCount, UINT _techID, UINT* _pVertexLayoutID);

	/***********************
	* CreateStaticBuffer: Creates a static buffer that holds both the Vertex and Index Buffers for the Object
	* @author: Jc Fowles
	* @parameter: _pVertices: The array of Vertices
	* @parameter: _pIndices: The array of Indices
	* @parameter: _vertexCount: Number of Vertices in the array of Vertices
	* @parameter: _indexCount: Number of Indices in the array of Indices
	* @parameter: _stride: memory byte size for each Vertex used as a stride
	* @parameter: _pBufferID: Reference ID created static buffer
	* @return: bool: Successful Creation
	********************/
	template<typename TIndices, typename TVertices>
	bool CreateStaticBuffer(typename TVertices* _pVertices, 
							typename TIndices* _pIndices,
							UINT _vertexCount, UINT _indexCount, 
							UINT _stride, UINT* _pBufferID)
	{
		*_pBufferID = ++nxtStaticBufferID;

		CStaticBuffer* staticBuff = new CStaticBuffer(m_pDX10Device);
		if (staticBuff->Initialise(_pVertices, _pIndices, _vertexCount, _indexCount, _stride, _pBufferID))
		{
			// Add created static buffer to the map
			std::pair<UINT, CStaticBuffer*> bufferPair(nxtStaticBufferID, staticBuff);
			VALIDATE(m_staticBuffers.insert(bufferPair).second);

			return true;
		}
		else
		{
			// Delete the failed static buffer memory
			_pBufferID = 0;
			ReleasePointer(staticBuff);
			
			// Returned Failed Creation
			return false;
		}
	}

	/***********************
	* RenderObject: Renders the Object to the target view port
	* @author: Jc Fowles
	* @Parameter: UINT _staticBufferID: Reference ID to the static buffer
	* @Parameter: UINT _vertexLayoutID: Reference ID to the Vertex layout
	* @Parameter: UINT _techniqueID: Reference ID to the shader technique
	* @return: bool: 
	********************/
	bool RenderObject(UINT _staticBufferID, UINT _vertexLayoutID, UINT _techniqueID);

	/***********************
	* ClearScreen: Clears the back buffer to the clear color and present to the screen
	* @author: Jc Fowles
	* @return: void
	********************/
	void PresentClearScreen();

	/***********************
	* StartRender: Readies the Back Buffer for the new frame by clearing it
	* @author: Jc Fowles
	* @return: void
	********************/
	void StartRender();

	/***********************
	* EndRender: Presents the Back buffer
	* @author: Jc Fowles
	* @return: void
	********************/
	void EndRender();
		
	/***********************
	* SetTopology: Set the topology used to render a object
	* @author: Jc Fowles
	* @Parameter: D3D_PRIMITIVE_TOPOLOGY _topology: topology type used to render a object
	* @return: void: 
	********************/
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY _topology);

	/***********************
	* RestoreDefaultDrawStates: Restore the Default States for Drawing
	* @author: Jc Fowles
	* @return: void
	********************/
	void RestoreDefaultDrawStates();

	/***********************
	* SetInputLayout: Set the Vertex Layout as the Input Layout on the Renderer
	* @author: Jc Fowles
	* @parameter: _vertexLayoutID: Vertex Layout ID to be used
	* @return: bool: Successful Set
	********************/
	bool SetInputLayout(UINT _vertexLayoutID);

	/***********************
	* GetTechnique: Returns the Technique for the passed in Reference ID
	* @author: Jc Fowles
	* @parameter: _techID: Reference ID of the Technique to return
	* @return: ID3D10EffectTechnique*: DX10 Technique
	********************/
	ID3D10EffectTechnique* GetTechnique(UINT _techID);
	
	/***********************
	* CalcViewMatrix:  Calculate the View Matrix for the render
	* @author: Jc Fowles
	* @Parameter: float3 _pos:  View Position vector
	* @Parameter: float3 _target: View Looking at vector
	* @Parameter: float3 _up: View Direction up vector
	* @return: void: 
	********************/
	void CalcViewMatrix(float3 _pos, float3 _target, float3 _up);
	
	/***********************
	* CalcProjMatrix: Calculate the Projection Matrix for the render
	* @author: Jc Fowles
	* @Parameter: float _fFov: The Field of view in radians
	* @Parameter: float _fNear: The closest the projection can be
	* @Parameter: float _fFar: The Furtherest the projection can be
	* @return: void: 
	********************/
	void CalcProjMatrix(float _fFov, float _fNear, float _fFar);
	
	/***********************
	* GetViewMatrix: Return the View Matrix
	* @author: Jc Fowles
	* @return: D3DXMATRIX*: The View Matrix
	********************/
	D3DXMATRIX* GetViewMatrix(){ return &m_matView; };

	/***********************
	* GetProjMatrix: Return the Projection Matrix
	* @author: Jc Fowles
	* @return: D3DXMATRIX*: The Projection Matrix
	********************/
	D3DXMATRIX* GetProjMatrix(){ return &m_matProj; };

	/***********************
	* GetFullScreenMode: Return the fullscreen mode
	* @author: Jc Fowles
	* @return: bool: Boolean value if full screen active
	********************/
	bool GetFullScreenMode(){ return m_fullScreen; };

protected:
private:

	// Member Variables
public:
protected:
private:

	// Window Variables
	HWND m_hWnd;
	int m_windowWidth;
	int m_windowHeight;
	bool m_fullScreen;
	

	// Rendering Matrices
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	// DX10 Variables
	ID3D10Device*    m_pDX10Device;
	IDXGISwapChain*  m_pDX10SwapChain;
	ID3D10RenderTargetView* m_pRenderTargetView;
	ID3D10DepthStencilView* m_pDepthStencilView;
	ID3D10Texture2D* m_pDepthStencilBuffer;

	bool m_wireFrame;

	// TO DO: Do i need to save these? to toggle wire frame??
	ID3D10RasterizerState* m_pRasterizerStates;
	D3D10_RASTERIZER_DESC m_rasterizerState;

	D3D10_DRIVER_TYPE m_DX10DriverType;
	D3DXCOLOR m_clearColor;

	// Effect Map
	UINT nxtEffectID;
	std::map<std::string, UINT> m_effectIDs;
	std::map<UINT, ID3D10Effect*> m_effects;
	
	// Technique Map
	UINT nxtTechID;
	// First Key is Effect ID, Second Is the ID to the Technique
	std::map<UINT, std::map<std::string, UINT>> m_techIDs;
	std::map<UINT, ID3D10EffectTechnique*> m_techniques;

	// Input Layout Map
	UINT nxtInputLayoutID;
	std::map<UINT, ID3D10InputLayout*> m_inputLayouts;

	// Static Buffer map
	UINT nxtStaticBufferID;
	std::map<UINT, CStaticBuffer*> m_staticBuffers;
	
};

#endif //__DX10RENDERER_H__
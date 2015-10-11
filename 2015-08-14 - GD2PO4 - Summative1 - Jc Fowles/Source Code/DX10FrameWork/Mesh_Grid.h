/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Mesh_Grid.h
* Description : The Mesh class for a grid object
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __MESH_GRID_H__
#define __MESH_GRID_H__

// Local includes 
#include "BaseMesh.h"
#include "DX10Renderer.h"
#include "GenUtilities.h"
#include "DX10Utilities.h"
#include "Vertex.h"

class CMesh_Grid :
	public CBaseMesh
{
	// Member Functions
public:
	
	/***********************
	* CMesh_Grid: Default Constructor for the mesh
	* @author: Jc Fowles
	* @return: : 
	********************/
	CMesh_Grid();
	
	/***********************
	* ~CMesh_Grid: TDefault destructor for the mesh
	* @author: Jc Fowles
	* @return: : 
	********************/
	~CMesh_Grid();

	/***********************
	* Initialise: Initialise the Mesh
	* @author: Jc Fowles
	* @Parameter: CDX10Renderer * _pRenderer: Pointer to the Render
	* @Parameter: UINT _width: Width of Grid
	* @Parameter: UINT _length: Length of Grid
	* @Parameter: D3DXCOLOR _color: Color of grid Verts
	* @return: bool: Successful Initialization
	********************/
	bool Initialise(CDX10Renderer* _pRenderer, UINT _width, UINT _length, D3DXCOLOR _color);
		
	/***********************
	* CreateGrid: Create the static Buffer for the Grid mesh
	* @author: Jc Fowles
	* @Parameter: UINT _width: Width of Grid
	* @Parameter: UINT _length: Length of Grid
	* @Parameter: D3DXCOLOR _color: Color of grid Verts
	* @return: bool: Successful Creation
	********************/
	bool CreateGrid(UINT _width, UINT _length, D3DXCOLOR _color);
		
	/***********************
	* GenerateVertices: Create the the Vertex Buffer
	* @author: Jc Fowles
	* @Parameter: TVertexColor** ppVertices: OUT pointer to a Vertex Buffer
	* @Parameter: UINT verticesAlongWidth: Width of Grid
	* @Parameter: UINT verticesAlongLength: Length of Grid
	* @return: void: 
	********************/
	void GenerateVertices(TVertexColor** ppVertices, UINT verticesAlongWidth, UINT verticesAlongLength);
	
	/***********************
	* GenerateIndices: Create the the Index Buffer
	* @author: Jc Fowles
	* @Parameter: UINT * * ppIndices: OUT pointer to a Index Buffer 
	* @Parameter: UINT verticesAlongWidth: Width of Grid
	* @Parameter: UINT verticesAlongLength: Length of Grid
	* @return: UINT: 
	********************/
	UINT GenerateIndices(UINT** ppIndices, UINT verticesAlongWidth, UINT verticesAlongLength);
		
	/***********************
	* Draw: Draw the Mesh
	* @author: Jc Fowles
	* @Parameter: UINT _vertexLayoutID: Reference ID to the Vertex layout
	* @Parameter: UINT _techniqueID: Reference ID to the shader technique
	* @return: void: 
	********************/
	void Draw(UINT _vertexLayoutID, UINT _techniqueID);


protected:
private:

	// Member Variables
public:
protected:
private:
	
};

#endif; //__MESH_GRID_H__
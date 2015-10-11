/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c)2005 - 2015 Media Design School
*
* File Name : Mesh_Cube.h
* Description : The Mesh class for a Cube object
* Author : Jc Fowles
* Mail : Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __MESH_CUBE_H__
#define __MESH_CUBE_H__

// Local Includes 
#include "BaseMesh.h"

class CMesh_Cube :
	public CBaseMesh
{
	// Member Functions
public:
	
	/***********************
	* CMesh_Cube: Default Constructor for the mesh
	* @author: Jc Fowles
	* @return: : 
	********************/
	CMesh_Cube();
	
	/***********************
	* ~CMesh_Cube: Default Destructor for the mesh
	* @author: Jc Fowles
	* @return: : 
	********************/
	~CMesh_Cube();
	
	/***********************
	* Initialise: Initialise the Mesh
	* @author: Jc Fowles
	* @Parameter: CDX10Renderer * _pRenderer: Pointer to the Render
	* @Parameter: D3DXCOLOR _color: Color of grid Verts
	* @return: bool: Successful Initialization
	********************/
	bool Initialise(CDX10Renderer* _pRenderer, D3DXCOLOR _color);

	/***********************
	* Initialise: Initialise the Mesh
	* @author: Jc Fowles
	* @Parameter: CDX10Renderer * _pRenderer: Pointer to the Render
	* @return: bool: Successful Initialization
	********************/
	bool Initialise(CDX10Renderer* _pRenderer);

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

#endif; //__MESH_CUBE_H__

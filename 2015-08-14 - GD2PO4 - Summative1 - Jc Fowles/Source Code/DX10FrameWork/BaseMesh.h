/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c)2005 - 2015 Media Design School
*
* File Name : BaseMesh.h
* Description : Base class for the all Mesh objects
* Author : Jc Fowles
* Mail : Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "DX10Renderer.h"
#include "GenUtilities.h"
#include "DX10Utilities.h"
#include "Vertex.h"

class CBaseMesh
{
public:

	/***********************
	* CMesh_Cube: Default Constructor for the mesh
	* @author: Jc Fowles
	* @return: :
	********************/
	CBaseMesh();

	/***********************
	* ~CMesh_Cube: Default Destructor for the mesh
	* @author: Jc Fowles
	* @return: :
	********************/
	~CBaseMesh();

	/***********************
	* Initialise: Initialise the Mesh
	* @author: Jc Fowles
	* @return: bool: Successful Initialization
	********************/
	bool Initialise();

	/***********************
	* Draw: Draw the Mesh
	* @author: Jc Fowles
	* @Parameter: UINT _vertexLayoutID: Reference ID to the Vertex layout
	* @Parameter: UINT _techniqueID: Reference ID to the shader technique
	* @return: void:
	********************/
	virtual void Draw(UINT _vertexLayoutID, UINT _techniqueID) = 0;

protected:
	CDX10Renderer* m_pRenderer;
	UINT m_staticBufferID;
};

#endif; //__MESH_H__
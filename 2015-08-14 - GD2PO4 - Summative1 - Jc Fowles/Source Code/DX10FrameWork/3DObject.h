/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : 3DObject.h
* Description : a basic 3D object class
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __3D_OBJECT_H__
#define __3D_OBJECT_H__

#include "BaseMesh.h"

class C3DObject
{
	// Member Functions
public:

	// Creation and Deletion

	/***********************
	* CCube: Default Constructor for Cube class
	* @author: Jc Fowles
	* @return: 
	********************/
	C3DObject();

	/***********************
	* ~CCube: Default Destructor for Cube class
	* @author: Jc Fowles
	* @return: 
	********************/
	~C3DObject();

	/***********************
	* Initialise: Initialise the 3D object
	* @author: Jc Fowles
	* @Parameter: CDX10Renderer * _pRenderer: pointer to the Render
	* @Parameter: D3D_PRIMITIVE_TOPOLOGY _topology: Topology used to draw object 
	* @Parameter: CBaseMesh * _pMesh: Pointer to the mesh this object will use
	* @Parameter: D3DXCOLOR _color: Color of the Object
	* @Parameter: float3 _scale:  3 float vector for the scale of the object 
	* @return: bool: Successful Initialization
	********************/
	bool Initialise(CDX10Renderer* _pRenderer, D3D_PRIMITIVE_TOPOLOGY _topology, CBaseMesh* _pMesh, D3DXCOLOR _color, float3 _scale);

	// Base Class Functions

	/***********************
	* BuildEffect: Build the effects to be used for the Geometric Object
	* @author: Jc Fowles
	* @Parameter: std::string _fxFileName: File name for the .fx file to be used for this object
	* @Parameter: std::string _technique: Shader Technique from the .fx file to be used for the object
	* @return: bool: Successful Build
	********************/
	virtual bool BuildEffect(std::string _fxFileName, std::string _technique);

	/***********************
	* SetEffectVariables: Get the effects variables from the render and set them to the object
	* @author: Jc Fowles
	* @return: bool: Successful Set
	********************/
	virtual bool SetEffectVariables();

	/***********************
	* BuildVertexLayout: Creates the Vertex Layout for the Geometric Object
	* @author: Jc Fowles
	* @return: bool: Successful Creation
	********************/
	virtual bool CreateVertexLayout();
	
	/***********************
	* Process: Process the Cube
	* @author: Jc Fowles
	* @parameter: _deltaTick: Time taken Since last frame
	* @return: void
	********************/
	void Process(float _deltaTick);

	/***********************
	* Draw: Draw the Cube
	* @author: Jc Fowles
	* @return: void
	********************/
	void Draw();

	/***********************
	* CalcWorldMatrix: Calculate the World Matrix for the cube
	* @author: Jc Fowles
	* @return: void
	********************/
	void CalcWorldMatrix();

	// Cube Functionality

	// Getters and Setters
		
	/***********************
	* GetPos: Get Object Position
	* @author: Jc Fowles
	* @return: float3: The Object's Position
	********************/
	float3 GetPos() const { return m_pos; }
	
	/***********************
	* SetPos: Set the Objects Position
	* @author: Jc Fowles
	* @Parameter: float3 _Pos: Position to set object too
	* @return: void: 
	********************/
	void SetPos(float3 _Pos) { m_pos = _Pos; }
		
	/***********************
	* GetRotation: Get the Change in Rotation of the Object
	* @author: Jc Fowles
	* @return: float3: The Change in Rotation of the Object
	********************/
	float3 GetRotation() const { return m_rotation; }
	
	/***********************
	* SetRotation: Set the object Change in Rotation
	* @author: Jc Fowles
	* @Parameter: float3 _rotation: The rotation to be set for the change (.x = pitch, .y= Yaw, .z = Roll)
	* @return: void: 
	********************/
	void SetRotation(float3 _rotation) { m_rotation = _rotation; }
		
	/***********************
	* GetScale: Get the Scale of the object 
	* @author: Jc Fowles
	* @return: float3: The Scale of the object 
	********************/
	float3 GetScale() const { return m_scale; }
	
	/***********************
	* SetScale: Set the Scale of the object 
	* @author: Jc Fowles
	* @Parameter: float3 _scale: Scale to set the object too
	* @return: void: 
	********************/
	void SetScale(float3 _scale) { m_scale = _scale; }

protected:
private:

	// Member Variables
public:
protected:
private:

	CDX10Renderer* m_pRenderer;
	eVertexType m_vertexType;
	UINT m_stride;

	float3 m_pos;
	
	float3 m_rotation;	//(x -> Pitch),(y -> Yaw),(z -> Roll)

	float3 m_scale;

	UINT m_fxID;
	UINT m_techniqueID;
	UINT m_vertexLayoutID;

	D3DXMATRIX m_matWorld;
	ID3D10EffectVariable* m_pTechVar_MatWorld;
	ID3D10EffectVariable* m_pTechVar_MatView;
	ID3D10EffectVariable* m_pTechVar_MatProj;
	ID3D10EffectVariable* m_pTechVar_Color;
	ID3D10EffectVariable* m_pTechVar_DeltaTime;

	UINT m_vertexCount;
	UINT m_indexCount;
	UINT m_staticBufferID;

	CBaseMesh* m_pMesh;
	
	D3DXCOLOR m_color;
	
	D3D_PRIMITIVE_TOPOLOGY m_topology;
};

#endif //__3D_OBJECT_H__
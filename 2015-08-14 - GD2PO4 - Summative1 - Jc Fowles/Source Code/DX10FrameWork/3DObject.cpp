/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : 3DObject.cpp
Description : a basic 3D object class
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

// This Include
#include "3DObject.h"

// Creation and Deletion

C3DObject::C3DObject()
{
	// Set the Vertex Type the Cube will use
	m_vertexType = VT_COLOR;

	// Set position to Origin
	m_pos = { 0.0f, 0.0f, 0.0f };

	// Set Rotation to no Rotation
	m_rotation = { 0.0f, 0.0f, 0.0f };

	// Set Scale to one
	m_scale = { 1.0f, 1.0f, 1.0f };

	// Set IDs to zero (which if left to Zero is an error indication)
	m_techniqueID = 0;
	m_fxID = 0;
	m_vertexLayoutID = 0;
	m_staticBufferID = 0;

	// Set all pointers to NULL
	m_pRenderer = 0;
	m_pTechVar_MatWorld = 0;
	m_pTechVar_MatView = 0;
	m_pTechVar_MatProj = 0;

}


C3DObject::~C3DObject()
{
}

bool C3DObject::Initialise(CDX10Renderer* _pRenderer, D3D_PRIMITIVE_TOPOLOGY _topology, CBaseMesh* _pCubeMesh, D3DXCOLOR _color, float3 _scale)
{
	// Initialise World matrix to the Identity matrix
	D3DXMatrixIdentity(&m_matWorld);

	// Set Topology of the Object
	m_topology = _topology;

	// Store the renderer on the Cube
	m_pRenderer = _pRenderer;

	// Store the Mesh;
	m_pMesh = _pCubeMesh;
	
	// Store the objects color
	m_color = _color;
	
	// Store the objects scale
	m_scale = _scale;
	
	// Return Successful Initialization
	return true;
}

bool C3DObject::BuildEffect(std::string _fxFileName, std::string _technique)
{
	//VALIDATE(m_pRenderer->BuildEffect("color.fx", "ColorTechnique", &m_fxID, &m_techniqueID));
	VALIDATE(m_pRenderer->BuildEffect(_fxFileName, _technique, &m_fxID, &m_techniqueID));

	VALIDATE(SetEffectVariables());
	VALIDATE(CreateVertexLayout());

	return true;
}

bool C3DObject::SetEffectVariables()
{
	m_pTechVar_MatWorld = m_pRenderer->GetEffectVariable(m_fxID, "matWorld");
	VALIDATE(m_pTechVar_MatWorld != 0);

	m_pTechVar_MatView = m_pRenderer->GetEffectVariable(m_fxID, "matView");
	VALIDATE(m_pTechVar_MatView != 0);

	m_pTechVar_MatProj = m_pRenderer->GetEffectVariable(m_fxID, "matProj");
	VALIDATE(m_pTechVar_MatProj != 0);

	m_pTechVar_Color = m_pRenderer->GetEffectVariable(m_fxID, "objColor");
	VALIDATE(m_pTechVar_Color != 0);

	m_pTechVar_DeltaTime = m_pRenderer->GetEffectVariable(m_fxID, "time");
	VALIDATE(m_pTechVar_DeltaTime != 0);

	return true;
}

bool C3DObject::CreateVertexLayout()
{
	VALIDATE(m_pRenderer->CreateVertexLayout(m_vertexType, m_techniqueID, &m_vertexLayoutID));

	return true;
}

void C3DObject::CalcWorldMatrix()
{
	// Transform Matrices Used to calculate the World Matrix
	D3DXMATRIX matScale;
	D3DXMATRIX matRotation;
	D3DXMATRIX matTranslation;

	// Initialise each transform matrix to the Identity Matrix
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matTranslation);

	// Reset the Objects World Matrix for new Calculation
	D3DXMatrixIdentity(&m_matWorld);

	// Create Scale Transform matrix
	D3DXMatrixScaling(&matScale, m_scale.x, m_scale.y, m_scale.z);

	// Create rotational Transform matrix
	D3DXMatrixRotationYawPitchRoll(&matRotation, m_rotation.y, m_rotation.x, m_rotation.z);

	// Calculate a translation Matrix based on the Objects current position in world space
	D3DXMatrixTranslation(&matTranslation, m_pos.x, m_pos.y, m_pos.z);

	// Calculate the Objects World Matrix
	// World = Scale * Rotation * Translation
	m_matWorld *= matScale;
	m_matWorld *= matRotation;
	m_matWorld *= matTranslation;
}

void C3DObject::Process(float _deltaTick)
{
	// Calculate the World Matrix
	CalcWorldMatrix();

	// Set All Technique variable on the shader
	m_pTechVar_MatWorld->AsMatrix()->SetMatrix((float*)m_matWorld);

	// Set the color of the object on the Shader
	HRESULT hr = m_pTechVar_Color->AsVector()->SetFloatVector((float*)m_color);
	
	
	hr = m_pTechVar_DeltaTime->AsScalar()->SetFloat(_deltaTick); //AsVector()->SetFloatVector((float*)m_color);

	if (_deltaTick > 500)
	{
		int c = 5;
	}

	// Set the View and projection on the shader
	m_pTechVar_MatView->AsMatrix()->SetMatrix((float*)*m_pRenderer->GetViewMatrix());
	m_pTechVar_MatProj->AsMatrix()->SetMatrix((float*)*m_pRenderer->GetProjMatrix());
}

void C3DObject::Draw()
{
	// Set the topology
	m_pRenderer->SetTopology(m_topology);
	// Draw the Object
	m_pMesh->Draw(m_vertexLayoutID, m_techniqueID);
}

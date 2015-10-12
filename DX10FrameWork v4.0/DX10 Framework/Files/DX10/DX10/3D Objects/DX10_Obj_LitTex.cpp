/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : DX10_Obj_LitTex.h
* Description : 3D Lit Texture Object for DirectX 10
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

// This Include
#include "DX10_Obj_LitTex.h"

DX10_Obj_LitTex::DX10_Obj_LitTex()
{
	DX10_Obj_Generic::BaseInitialise();

	m_pShader = 0;
	m_pTextures = 0;
	m_texIndex = 0;

	m_alphaReduce = 0.0f;

	m_timeElapsed = 0;
	m_animateTime = 0;
	m_animationSpeed = 0;
	m_scrollTime = 0;
	m_scrollSpeed = 0;

}

DX10_Obj_LitTex::~DX10_Obj_LitTex()
{
	ReleasePtr(m_pTextures);
}

bool DX10_Obj_LitTex::Initialise(DX10_Renderer* _pRenderer, DX10_Mesh* _pMesh, DX10_Shader_LitTex* _pShader, std::vector<std::string>* _pTexNames, float _animationSpeed)
{
	if (_pRenderer == 0 || _pMesh == 0 || _pShader == 0 || _pTexNames == 0 || _animationSpeed < 0)
	{
		// If any pointers are NULL, Object cannot be initialized
		return false;
	}

	if (_animationSpeed == 0.0f && _pTexNames->size() != 1)
	{
		return false;
	}

	// Assign Member Variables
	m_pRenderer = _pRenderer;
	m_pMesh = _pMesh;
	m_pShader = _pShader;
	m_texIndex = 0;
	m_animationSpeed = _animationSpeed;

	m_pTextures = new std::vector<ID3D10ShaderResourceView*>;
	for (UINT i = 0; i < _pTexNames->size(); i++)
	{
		ID3D10ShaderResourceView* pTempTex = 0;
		VALIDATE(m_pRenderer->CreateTexture((*_pTexNames)[i], pTempTex));
		m_pTextures->push_back(pTempTex);
	}
	// Delete allocated memory
	ReleasePtr(_pTexNames);

	m_processType = PT_ANIMATE;

	return true;
}

bool DX10_Obj_LitTex::Initialise(DX10_Renderer* _pRenderer, DX10_Mesh* _pMesh, DX10_Shader_LitTex* _pShader, std::string _texName, float _scrollTime, v2float _scrollDir)
{
	if (_pRenderer == 0 || _pMesh == 0 || _pShader == 0 || _scrollTime < 0)
	{
		// If any pointers are NULL, Object cannot be initialized
		return false;
	}
	
	// Assign Member Variables
	m_pRenderer = _pRenderer;
	m_pMesh = _pMesh;
	m_pShader = _pShader;
	m_texIndex = 0;
	m_scrollSpeed = _scrollTime;
	m_scrollDirection = _scrollDir.Normalise();
	m_alphaReduce = 0.2f;

	m_pTextures = new std::vector<ID3D10ShaderResourceView*>;
	ID3D10ShaderResourceView* pTempTex = 0;
	VALIDATE(m_pRenderer->CreateTexture(_texName, pTempTex));
	m_pTextures->push_back(pTempTex);

	m_processType = PT_SCROLL;

	return true;
}

bool DX10_Obj_LitTex::Initialise(DX10_Renderer* _pRenderer, DX10_Mesh* _pMesh, DX10_Shader_LitTex* _pShader, std::string _texName)
{
	if (_pRenderer == 0 || _pMesh == 0 || _pShader == 0)
	{
		// If any pointers are NULL, Object cannot be initialized
		return false;
	}

	// Assign Member Variables
	m_pRenderer = _pRenderer;
	m_pMesh = _pMesh;
	m_pShader = _pShader;

	m_pTextures = new std::vector<ID3D10ShaderResourceView*>;
	ID3D10ShaderResourceView* pTempTex = 0;
	VALIDATE(m_pRenderer->CreateTexture(_texName, pTempTex));
	m_pTextures->push_back(pTempTex);

	m_processType = PT_DEFUALT;

	return true;
}



void DX10_Obj_LitTex::Process(float _dt)
{
	BaseProcess(_dt);

	switch (m_processType)
	{
	case PT_DEFUALT:
	{
		m_timeElapsed += _dt;
		m_texIndex = 0;
	}
		break;
	case PT_SCROLL:
	{
		m_scrollTime += _dt;
		m_ScrollTexCoord = m_scrollDirection.Normalise() * (m_scrollTime / m_scrollSpeed);

		if (m_scrollTime >= m_scrollSpeed)
		{
			m_scrollTime -= m_scrollSpeed;
			m_texIndex = 0;
		}
	}
		break;
	case PT_ANIMATE:
	{
		m_animateTime += _dt;
		m_texIndex = (int)(m_timeElapsed * (float)m_pTextures->size() / m_animationSpeed);

		if (m_animateTime >= m_animationSpeed)
		{
			m_animateTime -= m_animationSpeed;
			m_texIndex = 0;
		}
	}
		break;
	}


}

void DX10_Obj_LitTex::Render(eTech_LitTex _tech)
{
	TLitTex litTex;
	ZeroMemory(&litTex, sizeof(litTex));
	litTex.pMesh = m_pMesh;
	litTex.pMatWorld = &m_matWorld;
	litTex.pTexBase = (*m_pTextures)[m_texIndex];
	litTex.scrollTexCoord = m_ScrollTexCoord;
	litTex.reduceAlpha = m_alphaReduce;

	if (_tech == TECH_LITTEX_FOG)
	{
		litTex.fogStart = 2.0f;
		litTex.fogRange = 200.0f;
		litTex.fogColor = m_pRenderer->GetClearColor();
	}

	m_pShader->Render(litTex, _tech);
}

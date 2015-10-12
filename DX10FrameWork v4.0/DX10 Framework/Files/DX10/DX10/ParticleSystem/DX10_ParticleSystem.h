/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : ParticleSystem.h
* Description : Code Used for creating, updating and drawing a particle system
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

// Inclusion Guards
#pragma once
#ifndef __DX10_PARTICLE_SYSTEM_H__
#define __DX10_PARTICLE_SYSTEM_H__

// Library Includes

// Local Includes
#include "../../../Utility/Utilities.h"
#include "../DX10_Utilities.h"

// Struct for the particle Vertex
struct ParticleVertex
{
	D3DXVECTOR3 initialPos;
	D3DXVECTOR3 initialVel;
	D3DXVECTOR2 size;
	float age;
	unsigned int type;
};


class DX10_ParticleSystem
{
	// Member functions
public:

	/***********************
	* DX10_ParticleSystem: Default Constructor for DX10 Particle System class
	* @author: Jc Fowles
	* @return:
	********************/
	DX10_ParticleSystem();

	/***********************
	* ~DX10_ParticleSystem: Default Destructor for DX10 Particle System class
	* @author: Jc Fowles
	* @return:
	********************/
	~DX10_ParticleSystem();

	/***********************
	* Initialise: Initialise the DX10 Particle System class
	* @author: Jc Fowles
	* @parameter: _pDX10Device: The DirectX 10 device 
	* @parameter: _pFX: the pointer to the created particle effect .fx file
	* @parameter: _texureArray: Pointer to an array of textures 
	* @parameter: _randomtexure: The random 1D texture used to get random values in the shader
	* @parameter: _pCameraPostion: Position of the camera in world space
	* @parameter: _pMatView: pointer to the view matrix
	* @parameter: _pMatProj: pointer to the projection matrix
	* @parameter: _maxParticles: max number of particles
	* @return: bool: Succesful initialization 
	********************/
	bool Initialise(ID3D10Device* _pDX10Device, ID3D10Effect* _pFX,
		ID3D10ShaderResourceView* _texureArray, ID3D10ShaderResourceView* _randomtexure,
		D3DXVECTOR3* _pCameraPostion, D3DXMATRIX* _pMatView, D3DXMATRIX* _pMatProj,
		UINT _maxParticles);


	/***********************
	* Reset: Restart the particle system
	* @author: Jc Fowles
	* @return: void:
	********************/
	void Reset();

	/***********************
	* Process: Process the DX10 Particle System
	* @author: Jc Fowles
	* @parameter: _dt: Delta tick time since last frame
	* @parameter: _gameTime: Time since the start of the game
	* @return: void:
	********************/
	void Process(float _dt, float _gameTime);

	/***********************
	* Render: Render the DX10 Particle System
	* @author: Jc Fowles
	* @return: void:
	********************/
	void Render();

	/***********************
	* SetEmitterPosition: Set the prime emitters position
	* @author: Jc Fowles
	* @parameter: _emitterPosition: 3D vector to set as the emitters position
	* @return: void:
	********************/
	void SetEmitterPosition(const v3float& _emitterPosition);

	/***********************
	* setEmitterDirection: Set the prime emitters Direction
	* @author: Jc Fowles
	* @parameter: _emitterPosition: 3D vector to set as the emitters Direction
	* @return: void:
	********************/
	void setEmitterDirection(const v3float& _emitterDirection);

protected:
private:

	DX10_ParticleSystem(const DX10_ParticleSystem& rhs);
	DX10_ParticleSystem& operator=(const DX10_ParticleSystem& rhs);

	/***********************
	* CreateFXVarPointers: Create the pointers to the FX memory on the graphics card
	* @author: Jc Fowles
	* @parameter: _pFX : Pointer to the .fx file to get the variables from
	* @return: bool: Successful or not
	********************/
	bool CreateFXVarPointers(ID3D10Effect* _pFX);

	/***********************
	* CreateVertexBuffers: Create the three vertex buffers
	* @author: Jc Fowles
	* @return: bool: Successful or not
	********************/
	bool CreateVertexBuffers();

	/***********************
	* CreateInputLayout: Create the Input layout for the vertex buffers
	* @author: Jc Fowles
	* @return: bool: Successful or not
	********************/
	bool CreateInputLayout();

	// Member Variables
public:
protected:
private:

	UINT m_maxParticles;
	bool m_firstRun;

	float m_gameTime;
	float m_timeStep;
	float m_age;
	
	D3DXMATRIX* m_pMatView;
	D3DXMATRIX* m_pMatProj;

	D3DXVECTOR3* m_pCameraPosition;

	D3DXVECTOR4 m_emitterPosition;
	D3DXVECTOR4 m_emitterDirection;

	ID3D10Device* m_pDX10Device;

	ID3D10InputLayout* m_pInputLayout;

	ID3D10Buffer* m_pInitialVB;
	ID3D10Buffer* m_pRenderVB;
	ID3D10Buffer* m_pStreamOutVB;

	ID3D10ShaderResourceView* m_pTexureArray;
	ID3D10ShaderResourceView* m_pRandomTexure;

	ID3D10Effect* m_pEffect;

	ID3D10EffectTechnique* m_pProcessTech;
	ID3D10EffectTechnique* m_pRenderTech;

	ID3D10EffectScalarVariable* m_pFXGameTimeVar;
	ID3D10EffectScalarVariable* m_pFXTimeStepVar;
	ID3D10EffectMatrixVariable* m_pFXMatViewVar;
	ID3D10EffectMatrixVariable* m_pFXMatProjVar;
	ID3D10EffectVectorVariable* m_pFXCameraPosVar;
	ID3D10EffectVectorVariable* m_pFXEmitterPosVar;
	ID3D10EffectVectorVariable* m_pFXEmitterDirVar;
	
	ID3D10EffectShaderResourceVariable* m_fxTextureArrayVar;
	ID3D10EffectShaderResourceVariable* m_fxRandomTextureVar;

};

#endif; //__DX10_PARTICLE_SYSTEM_H__

/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : CDebugCamera.h
* Description : Debug Camera used to move freely around the world
* Author : Jc Fowles
* Mail : Jc.Fowles@mediadesign.school.nz
*/


//This include
#include "DebugCamera.h"

CDebugCamera::CDebugCamera()
{
	
}

CDebugCamera::~CDebugCamera()
{

}

bool CDebugCamera::Initialise(CDX10Renderer* _pRenderer)
{
	m_pRenderer = _pRenderer;

	m_maxPitch = (89.0f);
	m_maxVelocity = 100.0f;
	m_moveSpeed = 10.0f;
	m_turnSpeed = 20.0f;

	m_position =	{ 0.0f, 0.0f, -15.0f };
	m_target =		{ 0.0f, 0.0f, 0.0f };
	m_velocity =	{ 0.0f, 0.0f, 0.0f };

	m_right =	{ 1.0f, 0.0f, 0.0f };
	m_up =		{ 0.0f, 1.0f, 0.0f };
	m_look =	{ 0.0f, 0.0f, 1.0f };
	
	Process();
		
	return true;
}

void CDebugCamera::SetViewProperties(float3 _Pos, float3 _target, float3 _up)
{
	m_position = _Pos;
	m_target = _target;
	m_up = _up;
}

void CDebugCamera::Process()
{	
	//Cap velocity to max velocity
	if (m_velocity.Magnitude() > m_maxVelocity)
	{
		m_velocity = m_velocity.Normalise() * m_maxVelocity;
	}
		
	//Move the camera
	m_position += m_velocity;
	
	//Stop continues movement
	m_velocity = m_velocity * 0.0f;
	m_look = m_look.Normalise();
	m_target = m_position + m_look;
		
	//Get the render to calculate the view matrix and set the view port
	m_pRenderer->CalcViewMatrix(m_position, m_target, m_up);

}

void CDebugCamera::Pitch(float _fDeg)
{
	float fRotInRads = (float)D3DXToRadian(_fDeg);
	fRotInRads = fRotInRads * m_turnSpeed;
	if (fRotInRads == 0.0f)
	{
		return;
	}
	
	D3DXMATRIX D3DXRotation;
	// Rotate around the cameras Right axis
	D3DXVECTOR3 tempRight = D3DXVECTOR3(m_right.x, m_right.y, m_right.z);
	D3DXMatrixRotationAxis(&D3DXRotation, &tempRight, (fRotInRads));

	// Calculate the new target and Up Vectors

	D3DXVECTOR3 tempUp = D3DXVECTOR3(m_up.x, m_up.y, m_up.z);
	D3DXVec3TransformNormal(&tempUp, &tempUp, &D3DXRotation);
	
	D3DXVECTOR3 tempLook = D3DXVECTOR3(m_look.x, m_look.y, m_look.z);
	D3DXVec3TransformNormal(&tempLook, &tempLook, &D3DXRotation);
	
	// Check if we we can rotate
	// Only Rotate if pitch does not go over max pitch
	float lookLengthOnXZ = sqrtf(tempLook.z * tempLook.z + tempLook.x * tempLook.x);
	float m_fPitch = (float)D3DXToDegree(atan2f(tempLook.y, lookLengthOnXZ));

	bool bRotate = true;
	if (m_fPitch > m_maxPitch)
	{
		bRotate = false;
	}
	else if (m_fPitch < -m_maxPitch)
	{
		bRotate = false;
	}

	if (bRotate)
	{
		m_up = float3(tempUp.x, tempUp.y, tempUp.z);
		m_look = float3(tempLook.x, tempLook.y, tempLook.z);
	}
}

void CDebugCamera::Yaw(float _fDeg)
{
	float fRotInRads = (float)D3DXToRadian(_fDeg);
	fRotInRads = fRotInRads * m_turnSpeed;
	
	if (fRotInRads == 0.0f)
	{
		return;
	}

	D3DXMATRIX D3DXRotation;
	// Rotate around the cameras Up axis
	D3DXVECTOR3 tempUp = D3DXVECTOR3(m_up.x, m_up.y, m_up.z);
	D3DXMatrixRotationAxis(&D3DXRotation, &tempUp, (fRotInRads));

	// Calculate the new target and Right Vectors

	D3DXVECTOR3 tempRight = D3DXVECTOR3(m_right.x, m_right.y, m_right.z);
	D3DXVec3TransformNormal(&tempRight, &tempRight, &D3DXRotation);
	m_right = float3(tempRight.x, tempRight.y, tempRight.z);

	D3DXVECTOR3 tempLook = D3DXVECTOR3(m_look.x, m_look.y, m_look.z);
	D3DXVec3TransformNormal(&tempLook, &tempLook, &D3DXRotation);
	m_look = float3(tempLook.x, tempLook.y, tempLook.z);
}

void CDebugCamera::Roll(float _fDeg)
{
	float fRotInRads = (float)D3DXToRadian(_fDeg);
	fRotInRads = fRotInRads * m_turnSpeed;
	if (fRotInRads == 0.0f)
	{
		return;
	}

	D3DXMATRIX D3DXRotation;
	// Rotate around the cameras look axis
	D3DXVECTOR3 tempLook = D3DXVECTOR3(m_look.x, m_look.y, m_look.z);
	D3DXMatrixRotationAxis(&D3DXRotation, &tempLook, (fRotInRads));

	// Calculate the new up and Right Vectors

	D3DXVECTOR3 tempRight = D3DXVECTOR3(m_right.x, m_right.y, m_right.z);
	D3DXVec3TransformNormal(&tempRight, &tempRight, &D3DXRotation);
	m_right = float3(tempRight.x, tempRight.y, tempRight.z);

	D3DXVECTOR3 tempUp = D3DXVECTOR3(m_up.x, m_up.y, m_up.z);
	D3DXVec3TransformNormal(&tempUp, &tempUp, &D3DXRotation);
	m_up = float3(tempUp.x, tempUp.y, tempUp.z);

}

void CDebugCamera::Strafe(float _fDistance)
{
	_fDistance = _fDistance * m_moveSpeed;
	m_velocity += m_right * _fDistance;
}

void CDebugCamera::Fly(float _fDistance)
{
	_fDistance = _fDistance * m_moveSpeed;
	m_velocity += m_up * _fDistance;
}

void CDebugCamera::Move(float _fDistance)
{
	_fDistance = _fDistance * m_moveSpeed;
	m_velocity += m_look * _fDistance;
	
}


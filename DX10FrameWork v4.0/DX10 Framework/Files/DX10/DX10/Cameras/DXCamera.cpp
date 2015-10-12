// 
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2015 Media Design School
//
// File Name	:	DXCamera.cpp
// Description	:	The implementation file.
// Author(s)	:	Juran Griffith.
// Mail			:	juran.griffith@mediadesignschool.com
//

// Local Includes
#include "DXCamera.h"

DXCamera::DXCamera() :
	m_vec3DefaultPosition(0.0f, 0.0f, -10.0f),
	m_vec3DefaultTarget(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_vec3DefaultUp(D3DXVECTOR3(0.0f, 1, 0.0f)),
	m_vec3DefaultForward(D3DXVECTOR3(0.0f, 0.0f, 1.0f)),
	m_vec3DefaultRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),

	m_vec3Position(D3DXVECTOR3(0.0f, 0.0f, -10.0f)),
	m_vec3Target(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_vec3Up(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
	m_vec3Forward(D3DXVECTOR3(0.0f, 0.0f, 1.0f)),
	m_vec3Right(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),

	m_yaw(0.0f),
	m_pitch(0.0f),
	m_moveLR(0.0f),
	m_moveFB(0.0f)
{
	// Set matrices to identity
	D3DXMatrixIdentity(&m_matrixView);
	D3DXMatrixIdentity(&m_matrixProjection);
}

DXCamera::~DXCamera()
{

}

bool DXCamera::Initialize(float _fov, float _aspect, float _zNear, float _zFar, D3DXVECTOR3 _position, D3DXVECTOR3 _target, eCamera _type)
{
	SetPosition(_position);
	SetTarget(_target);
	m_vec3DefaultPosition = _position;
	m_vec3DefaultTarget = _target;
	m_type = _type;

	D3DXMatrixLookAtLH(&m_matrixView, &m_vec3Position, &m_vec3Target, &m_vec3DefaultUp);

	D3DXMatrixPerspectiveFovLH(&m_matrixProjection, _fov, _aspect, _zNear, _zFar);

	return true;
}

void DXCamera::SetPosition(D3DXVECTOR3 _position)
{
	m_vec3Position = _position;
}

void DXCamera::SetTarget(D3DXVECTOR3 _target)
{
	m_vec3Target = _target;
}

void DXCamera::SetMove(float _deltaMove, eMove _direction)
{
	switch (_direction)
	{
		case eMove::EMOVE_LEFT_RIGHT:
		{
			m_moveLR += _deltaMove;
		}
		break;
		case eMove::EMOVE_FORWARD_BACK:
		{
			m_moveFB += _deltaMove;
		}
		break;
	}
}

void DXCamera::SetYawPitch(float _yaw, float _pitch)
{
	m_yaw += _yaw;
	m_pitch += _pitch;
}

void DXCamera::Update(float _dt)
{
	D3DXMatrixRotationYawPitchRoll(&m_matrixRotation, m_yaw, m_pitch, 0);
	D3DXVec3TransformCoord(&m_vec3Target, &m_vec3DefaultForward, &m_matrixRotation);
	D3DXVec3Normalize(&m_vec3Target, &m_vec3Target);

	switch (m_type)
	{
		case eCamera::ECAMERA_DEFAULT:
		{
			D3DXVec3TransformNormal(&m_vec3Right, &m_vec3DefaultRight, &m_matrixRotation);
			D3DXVec3TransformNormal(&m_vec3Forward, &m_vec3DefaultForward, &m_matrixRotation);
			D3DXVec3Cross(&m_vec3Up, &m_vec3Forward, &m_vec3Right);
		}
		break;
		case eCamera::ECAMERA_FPS:
		{
			D3DXMATRIX RotateYTempMatrix;
			D3DXMatrixRotationY(&RotateYTempMatrix, m_yaw);

			D3DXVec3TransformNormal(&m_vec3Right, &m_vec3DefaultRight, &RotateYTempMatrix);
			D3DXVec3TransformNormal(&m_vec3Up, &m_vec3DefaultUp, &RotateYTempMatrix);
			D3DXVec3TransformNormal(&m_vec3Forward, &m_vec3DefaultForward, &RotateYTempMatrix);
		}
		break;
		case eCamera::ECAMERA_THIRD:
		{
			// TODO
		}
		break;
	}
	
	// Update position
	m_vec3Position += m_moveLR * m_vec3Right * _dt;
	m_vec3Position += m_moveFB * m_vec3Forward * _dt;

	// Reset the movement
	m_moveLR = 0.0f;
	m_moveFB = 0.0f;

	// Adjust the target
	m_vec3Target = m_vec3Position + m_vec3Target;

	D3DXMatrixLookAtLH(&m_matrixView, &m_vec3Position, &m_vec3Target, &m_vec3Up);
}

D3DXMATRIX& DXCamera::GetViewMatrix()
{
	return m_matrixView;
}

D3DXMATRIX& DXCamera::GetProjectionMatrix()
{
	return m_matrixProjection;
}
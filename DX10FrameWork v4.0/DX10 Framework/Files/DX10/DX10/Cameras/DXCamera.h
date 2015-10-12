//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2015 Media Design School
//
// File Name	:	DXCamera.h
// Description	:	This wraps all the camera functions and variables
// Author		:	Juran Griffith
// Mail			:	juran.griffith@mediadesignschool.com
//


#pragma once
#ifndef __DX_CAMERA_H__
#define __DX_CAMERA_H__

// Library Includes

// Local Includes
#include "../DX10_Utilities.h"
//#include "../../../Utility/Utilities.h"


enum eCamera
{
	ECAMERA_DEFAULT,
	ECAMERA_FPS,
	ECAMERA_THIRD
};

enum eMove
{
	EMOVE_FORWARD_BACK,
	EMOVE_LEFT_RIGHT
};

class DXCamera
{
	// Functions
	public:
		/*******************
		-> Default constructor.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	...
		********************/
		DXCamera();

		/*******************
		-> Default destructor.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	...
		********************/
		~DXCamera();

		/*******************
		-> Intializes the object
		@author:	Juran Griffith.
		@parameter:	_fov		- The field of view.
		@parameter:	_aspect		- The aspect ratio of the screen.
		@parameter:	_zNear		- The z near plane.
		@parameter:	_zFar		- The z far plane.
		@parameter:	_position	- The new position for the camera.
		@parameter:	_target		- The new target to look at.
		@parameter:	_type		- The type of movement this camera will have. By default its a free moving camera.
		@return:	bool		- Returns true if the intialization succeeds otherwise the error on why it failed.
		********************/
		bool Initialize(float _fov, float _aspect, float _zNear, float _zFar, D3DXVECTOR3 _position, D3DXVECTOR3 _target, eCamera _type = ECAMERA_DEFAULT);

		/*******************
		-> Set the position of the camera
		@author:	Juran Griffith.
		@parameter:	_position	- The new position for the camera.
		@return:	void.
		********************/
		void SetPosition(D3DXVECTOR3 _position);

		/*******************
		-> Set the target to look at.
		@author:	Juran Griffith.
		@parameter:	_target	- The new target to look at.
		@return:	void.
		********************/
		void SetTarget(D3DXVECTOR3 _target);

		/*******************
		-> Set the change in movement for the camera. Use a negative delta to move in the opposite direction. 
		   e.g -1.0f for the movement using EMOVE_FORWARD_BACK direction, will move backwards.
		@author:	Juran Griffith.
		@parameter:	_deltaMove	- The new change in movement.
		@parameter: _direction	- The direcion to move in.
		@return:	void.
		********************/
		void SetMove(float _deltaMove, eMove _direction);

		/*******************
		-> Set the yaw and pitch for the camera
		@author:	Juran Griffith.
		@parameter:	_yaw	- The new change in yaw.
		@parameter:	_yaw	- The new change in pitch.
		@return:	void.
		********************/
		void SetYawPitch(float _yaw, float _pitch);

		/*******************
		-> Update the camera position and view based on delta time.
		@author:	Juran Griffith.
		@parameter:	_dt	- The change in time.
		@return:	void.
		********************/
		void Update(float _dt);

		/*******************
		-> Get the view matrix for the camera.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	D3DXMATRIX - The view matrix.
		********************/
		D3DXMATRIX& GetViewMatrix();

		/*******************
		-> Get the projection matrix for the camera.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	D3DXMATRIX - The view matrix.
		********************/
		D3DXMATRIX& GetProjectionMatrix();
	protected:
	private:

	// Variables
	public:
	protected:
	private:
		// View parameters in radians
		float				m_yaw;
		float				m_pitch;
		float				m_moveLR;	// Left & Right
		float				m_moveFB;	// Forward & Back

		eCamera				m_type;

		// Matrices
		D3DXMATRIX			m_matrixView;
		D3DXMATRIX			m_matrixProjection;
		D3DXMATRIX			m_matrixRotation;

		// View vectors
		D3DXVECTOR3			m_vec3DefaultPosition;
		D3DXVECTOR3			m_vec3DefaultTarget;
		D3DXVECTOR3			m_vec3DefaultUp;
		D3DXVECTOR3			m_vec3DefaultForward;
		D3DXVECTOR3			m_vec3DefaultRight;

		D3DXVECTOR3			m_vec3Position;
		D3DXVECTOR3			m_vec3Target;
		D3DXVECTOR3			m_vec3Up;
		D3DXVECTOR3			m_vec3Forward;
		D3DXVECTOR3			m_vec3Right;
};

#endif
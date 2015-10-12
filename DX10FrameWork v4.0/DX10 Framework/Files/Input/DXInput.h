//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2015 Media Design School
//
// File Name	:	DXInput.h
// Description	:	This wraps all the direct input functions and variables
// Author		:	Juran Griffith
// Author		:	Jc Fowles (Edited)
// Mail			:	juran.griffith@mediadesignschool.com
//


#pragma once
#ifndef __DX_INPUT_H__
#define __DX_INPUT_H__

// Linker Includes
#pragma comment (lib, "dinput8.lib")

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif

// Local Includes
#include "../Utility/Utilities.h"
#include "../DX10/DX10/DX10_Utilities.h"
// Library Includes
#include <dinput.h>

class DXInput
{
	// Functions
	public:
		/*******************
		-> Default constructor.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	...
		********************/
		DXInput();

		/*******************
		-> Default destructor.
		@author:	Juran Griffith.
		@author:	Jc Fowles (Edited).
		@parameter:	None.
		@return:	...
		********************/
		~DXInput();

		/*******************
		-> This intializes the direct input.
		@author:	Juran Griffith.
		@author:	Jc Fowles (Edited).
		@parameter:	_pHWnd		- The handler to the window.
		@parameter:	_hInstance	- The instance handler to the current application.
		@return:	bool		- Returns true if successful.
		********************/
		bool Initialize(HWND* _pHWnd, HINSTANCE* _phInstance);

		/*******************
		-> This reads all direct input.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	void.
		********************/
		void ReadInput();

		/*******************
		-> This gets the keyboard device.
		@author:	Juran Griffith.
		@parameter:	IDirectInputDevice8* - The pointer to the keyboard device
		@return:	void.
		********************/
		//IDirectInputDevice8* GetKeyBoard();

		/*******************
		-> This gets the mouse device.
		@author:	Juran Griffith.
		@parameter:	IDirectInputDevice8* - The pointer to the mouse device
		@return:	void.
		********************/
		//IDirectInputDevice8* GetMouse();

		/*******************
		-> This gets the keyboards current state
		@author:	Juran Griffith.
		@parameter:	BYTE* - The pointer to the keyboard data
		@return:	void.
		********************/
		BYTE* GetKeyboardState();

		/*******************
		-> This gets the current mouse state.
		@author:	Juran Griffith.
		@parameter:	DIMOUSESTATE - The current mouse data.
		@return:	void.
		********************/
		DIMOUSESTATE GetMouseState();

		/*******************
		-> This gets the previous mouse state
		@author:	Juran Griffith.
		@parameter:	DIMOUSESTATE - The current mouse data.
		@return:	void.
		********************/
		DIMOUSESTATE GetPreviousMouseState();

		/*******************
		-> This gets the current mouse position
		@author:	Juran Griffith.
		@author:	Jc Fowles (Edited).
		@parameter:	_out	- The point result of the mouse posiiton. 
		@return:	void.
		********************/
		void GetMousePosition(v2float& _out);

		/*******************
		-> Saves the current mouse state, updating the previous state.
		@author:	Juran Griffith.
		@parameter:	None.
		@return:	void.
		********************/
		void SetPreviousState();

		/*******************
		-> Checks to see if the current key is locked / pressed. This is unlocked when the button is released.
		@author:	Juran Griffith.
		@parameter:	_key	- The keyboard key to check if it is still pressed.
		@return:	void.
		********************/
		bool IsKeyLocked(DWORD _key);

		/*******************
		-> Locks the key passed to this function.
		@author:	Juran Griffith.
		@parameter:	_key	- The keyboard key that needs to be locked.
		@return:	void.
		********************/
		void LockKey(DWORD _key);

		/*******************
		-> Checks to see if the current mouse button is locked / pressed. This is unlocked when the button is released.
		@author:	Juran Griffith.
		@parameter:	_index	- The mouse index to check.
		@return:	void.
		********************/
		bool IsMouseBtnLocked(int _index);

		/*******************
		-> Locks the selected mouse button. This is unlocked when the button is released.
		@author:	Juran Griffith.
		@parameter:	_index	- The mouse index to lock.
		@return:	void.
		********************/
		void LockMouseBtn(int _index);

	protected:
	private:

	// Variables
	public:
	protected:
	private:
		HWND*					m_pHWnd;
		HINSTANCE*				m_phInstance;
		int						m_screenWidth;
		int						m_screenHeight;

		IDirectInputDevice8*	m_pDIKeyboard;
		IDirectInputDevice8*	m_pDIMouse;
		IDirectInput8*			m_pDirectInput;

		BYTE					m_keyboardPressed[256];
		bool					m_keyLock[256];

		DIMOUSESTATE			m_mouseState, m_mousePreState;
		bool					m_mouseBtnLock[4];
		float					m_mouseX;
		float					m_mouseY;
		float					m_mousePrevX;
		float					m_mousePrevY;
};

#endif
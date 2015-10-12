// 
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2015 Media Design School
//
// File Name	:	DXInput.cpp
// Description	:	The implementation file.
// Author(s)	:	Juran Griffith.
// Author		:	Jc Fowles (Edited)
// Mail			:	juran.griffith@mediadesignschool.com
//

// Local Includes
#include "DXInput.h"

DXInput::DXInput()
{

}

DXInput::~DXInput()
{
		m_pDIMouse->Unacquire();
		m_pDIKeyboard->Unacquire();
	
	ReleasePtr(m_pDIMouse);
	ReleasePtr(m_pDIKeyboard);
	ReleasePtr(m_pDirectInput);
}

bool DXInput::Initialize(HWND* _pHWnd, HINSTANCE* _phInstance)
{
	m_pHWnd = _pHWnd;
	m_phInstance = _phInstance;

	// Intialize the direct input interface
	VALIDATEHR(DirectInput8Create(*m_phInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL))

	// Intialize the keyboard interface
	VALIDATEHR(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL));

	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pDIKeyboard->SetCooperativeLevel(*m_pHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_pDIKeyboard->Acquire();

	// Intialize the mouse interface
	VALIDATEHR(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL))

	m_pDIMouse->SetDataFormat(&c_dfDIMouse);
	m_pDIMouse->SetCooperativeLevel(*m_pHWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	m_pDIMouse->Acquire();

	m_mouseX = 0;
	m_mouseY = 0;

	RECT rect;
	if (GetClientRect(*m_pHWnd, &rect))
	{
		m_screenWidth = rect.right - rect.left;
		m_screenHeight = rect.bottom - rect.top;
	}

	return true;
}

void DXInput::ReadInput()
{
	HRESULT hr;

	// Read the keyboard device.
	hr = m_pDIKeyboard->GetDeviceState(sizeof(m_keyboardPressed), (LPVOID)&m_keyboardPressed);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pDIKeyboard->Acquire();
		}
	}

	// Read the mouse device.
	hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(hr))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pDIMouse->Acquire();
		}
	}
	else
	{
		// Update mouse position
		m_mouseX += static_cast<float>(m_mouseState.lX);
		m_mouseY += static_cast<float>(m_mouseState.lY);

		// Ensure the mouse is within the window bounds
		if (m_mouseX < 0.0f)  
			m_mouseX = 0.0f;
		if (m_mouseY < 0.0f)
			m_mouseY = 0.0f;

		if (static_cast<int>(m_mouseX) > m_screenWidth)
			m_mouseX = static_cast<float>(m_screenWidth);
		if (static_cast<int>(m_mouseY) > m_screenHeight)
			m_mouseY = static_cast<float>(m_screenHeight);
	}

	// Update the locked keys if the button is released.
	for (int i = 0; i < 256; ++i)
	{
		if (m_keyLock[i])
		{
			if (!(m_keyboardPressed[i] & 0x80))
			{
				m_keyLock[i] = false;
			}
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (m_mouseBtnLock[i])
		{
			if (!m_mouseState.rgbButtons[i] /*& 0x80*/)
			{
				m_mouseBtnLock[i] = false;
			}
		}
	}
}

BYTE* DXInput::GetKeyboardState()
{
	return m_keyboardPressed;
}

DIMOUSESTATE DXInput::GetMouseState()
{
	return m_mouseState;
}

DIMOUSESTATE DXInput::GetPreviousMouseState()
{
	return m_mousePreState;
}

void DXInput::GetMousePosition(v2float& _out)
{
	_out.x = m_mouseX;
	_out.y = m_mouseY;
}

void DXInput::SetPreviousState()
{
	m_mousePreState = m_mouseState;
}

bool DXInput::IsKeyLocked(DWORD _key)
{
	return m_keyLock[_key];
}

void DXInput::LockKey(DWORD _key)
{
	m_keyLock[_key] = true;
}

bool DXInput::IsMouseBtnLocked(int _index)
{
	if (_index < 4)
	{
		return m_mouseBtnLock[_index];
	}
	return false;
}

void DXInput::LockMouseBtn(int _index)
{
	m_mouseBtnLock[_index] = true;
}
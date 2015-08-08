/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : Application.h
* Description : The core of the Application is run here. The link between all components. //TO DO
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __APPLICATION_H__
#define __AAPLICATION_H__

// Defines

// Windows Defines
#define WIN32_LEAN_AND_MEAN
#define WINDOWS_CLASS_NAME L"DX10 FRAMEWORK"

// Debug Defines
#ifdef _DEBUG
// Run Visual Leak Detector in DEBUG mode 
#include "vld.h"
#define D3D_DEBUG_INFO
#endif // _DEBUG

// Library Includes 
#include <windows.h>

// Local Includes
#include "GenUtilities.h"
//#include "DX10RenderManager.h"


class CApplication
{
	//Functions
public:
	
	// *** Windows Functions ***

	/***********************
	* WindowProc: This is the message handler for the Window, 
	* @author: Jc Fowles
	* @Parameter: _hWnd: Handle to the Window sending the message
	* @Parameter: _uiMsg: The message ID being sent
	* @Parameter: _wParam: Additional detail about the message being sent
	* @Parameter: _lParam: Additional detail about the message being sent
	* @return: LRESULT: Program termination value
	********************/
	static LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam);
		
	/***********************
	* CreateAndRegisterWindow: Create and register the window of the application 
	* @author: Jc Fowles
	* @Parameter: _hInstance: Instance handler for the application
	* @Parameter: _iWidth: Width of the window
	* @Parameter: _iHeight: Height of the window
	* @Parameter: _pcTitle: Title of the window
	* @return: bool: Succesful Creation
	********************/
	bool CreateAndRegisterWindow(HINSTANCE _hInstance, int _iWidth, int _iHeight, LPCWSTR _pcTitle);
	
	/***********************
	* RunApp: Runs the main application loop
	* @author: Jc Fowles
	* @return: int: Program termination value
	********************/
	int RunApp();

	// *** Appliction Functions ***

	// Creation and Deletion
	
	/***********************
	* ~CApplication: Default Destructor for Application class
	* @author: Jc Fowles
	* @return:
	********************/
	~CApplication();

	/***********************
	* GetInstance: Returns the singleton instance of the Application, or creates it if it doesn't exist.
	* @author: Jc Fowles
	* @return: CApplication*: Pointer to the application Instance
	********************/
	static CApplication* GetInstance();

	/***********************
	* Initialise: Initialise the Application
	* @author: Jc Fowles
	* @return: bool: Successful Initialisation
	********************/
	bool Initialise();

	/***********************
	* ShutDown: Shuts down the appliction and releases all memory Created by it.
	* @author: Jc Fowles
	* @return: void
	********************/
	void ShutDown();
	
	/***********************
	* DestroyInstance: Deletes the instance of the Application.
	* @author: Jc Fowles
	* @return: void
	********************/
	static void DestroyInstance();
	
	// Application Functionality

	/***********************
	* ExecuteOneFrame: Executes a single frame of the Application
	* @author: Jc Fowles
	* @return: void
	********************/
	void ExecuteOneFrame();
	
protected:
private:
	//Singleton Methods to prevent multiple instances of this class
	CApplication();
	CApplication(const CApplication& _kr);
	CApplication& operator= (const CApplication& _kr);

	//Member Variables
public:
protected:
private:

	// Singleton Instance of the Application 
	static CApplication* s_pApp;

	// Window Variables
	HWND m_hWnd;
	int m_windowWidth;
	int m_windowHeight;


};


#endif //__APPLICATION_H__
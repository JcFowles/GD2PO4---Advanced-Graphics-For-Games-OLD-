/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : GenUtilities.h
* Description : General Utilitis used by the application
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

#pragma once
#ifndef __GEN_UTILS_H__
#define __GEN_UTILS_H__
#pragma message("Included General Utilities")

// Library Inlcudes 
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>
#include <math.h>
#include <map>

// Constants

const float kPI = 3.14159265358979323f;

// Defines 
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
// Success Check
// TO DO: assert on validates??
#define VALIDATE(a) { if (!a){ return (false); } }

// Pointer Deletion
#define ReleasePointer(x) { if (x){ delete x; x = 0; } }

/***********************
* float3: Struct to hold 3 float values a vector
* @author: Jc Fowles
* @return:
********************/
struct float3
{
	
	float x;
	float y;
	float z;

	/***********************
	* float3: constructor for the float3 type
	* @author: Jc Fowles
	* @Parameter: float _x: float to be stored in the x value of the float3
	* @Parameter: float _y: float to be stored in the y value of the float3
	* @Parameter: float _z: float to be stored in the z value of the float3
	* @return: :
	********************/
	float3(float _x = 0, float _y = 0, float _z = 0)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	/***********************
	* operator=: Assignment operator
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: value for 'this' to be assigned to
	* @return: float3: return the new assigned float3
	********************/
	float3 operator= (const float3 _f3)
	{
		x = _f3.x;
		y = _f3.y;
		z = _f3.z;
		return *this;
	}

	/***********************
	* operator+: Addition operator (vector)
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: right hand side of the operator
	* @return: float3: The result of the addition
	********************/
	float3 operator+ (const float3 _f3) const
	{
		return float3(_f3.x + x, _f3.y + y, _f3.z + z);
	}

	/***********************
	* operator+: Addition operator (scalar)
	* @author: Jc Fowles
	* @Parameter: const float _f: right hand side of the operator
	* @return: float3: The result of the addition
	********************/
	float3 operator+ (const float _f) const
	{
		return float3(_f + x, _f + y, _f + z);
	}

	/***********************
	* operator+: Addition and assignment operator
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: right hand side of the operator
	* @return: float3: The result of the addition and assignment
	********************/
	float3 operator+= (const float3 _f3)
	{
		return float3(x += _f3.x, y += _f3.y, z += _f3.z);
	}

	/***********************
	* operator-: Subtraction operator (vector)
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: right hand side of the operator
	* @return: float3: The result of the subtraction
	********************/
	float3 operator- (const float3 _f3) const
	{
		return float3(x - _f3.x, y - _f3.y, z - _f3.z);
	}

	/***********************
	* operator-: Subtraction operator (scalar)
	* @author: Jc Fowles
	* @Parameter: const float _f: right hand side of the operator
	* @return: float3: The result of the subtraction
	********************/
	float3 operator- (const float _f) const
	{
		return float3(x - _f, y - _f, z - _f);
	}

	/***********************
	* operator-=: Subtraction and assignment operator
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: right hand side of the operator
	* @return: float3: The result of the Subtraction and assignment
	********************/
	float3 operator-= (const float3 _f3)
	{
		return float3(x -= _f3.x, y -= _f3.y, z -= _f3.z);
	}

	/***********************
	* operator*: Multiplication operator
	* @author: Jc Fowles
	* @Parameter: const float3 _f3: right hand side of the operator
	* @return: float3: The result of the Multiplication
	********************/
	float3 operator* (const float3 _f3) const
	{
		return float3(_f3.x * x, _f3.y * y, _f3.z * z);
	}

	/***********************
	* operator*: Multiplication operator
	* @author: Jc Fowles
	* @Parameter: const float _f: right hand side of the operator
	* @return: float3: The result of the Multiplication
	********************/
	float3 operator* (const float _f) const
	{
		return float3(_f * x, _f * y, _f * z);
	}



	/***********************
	* operator*: Multiplication operator
	* @author: Jc Fowles
	* @Parameter: const float _f: right hand side of the operator
	* @return: float3: The result of the Multiplication
	********************/
	float3 operator/ (const float _f) const
	{
		return float3(x / _f, y / _f, z / _f);
	}

	/***********************
	* operator==: Equal comparison operator
	* @author: Jc Fowles
	* @Parameter: const float _f: right hand side of the operator
	* @return: bool: The result of the comparison
	********************/
	bool operator== (const float3 _f3) const
	{
		if ((x == _f3.x) &&
			(y == _f3.y) &&
			(z == _f3.z))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/***********************
	* Magnitude: Calculate and return the magnitude of the float3 vector
	* @author: Jc Fowles
	* @return: float: The magnitude of the float3 vector
	********************/
	float Magnitude()
	{
		return (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
	}

	/***********************
	* Normalise: Calculate and return a normalized vector of the float3 vector
	* @author: Jc Fowles
	* @return: float3: The normalized float3 vector
	********************/
	float3 Normalise()
	{
		float mag = this->Magnitude();
		if (mag == 0.0f)
		{
			// Return a zero vector if magnitude is zero
			return float3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			// Return the normalized vector 
			this->x = (this->x / mag);
			this->y = (this->y / mag);
			this->z = (this->z / mag);

			return (*this);
		}
	}

	/***********************
	* Limit: limit the magnitude of a vector
	* @author: Jc Fowles
	* @return: float3: The limit float3 vector
	********************/
	float3 Limit(float _fLimit)
	{
		float mag = this->Magnitude();
		if (mag > _fLimit)
		{
			(*this) = this->Normalise() * _fLimit;
		}
		return (*this);
	}

};

#endif //__GEN_UTILS_H__
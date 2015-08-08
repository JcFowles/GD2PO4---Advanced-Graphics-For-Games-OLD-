//=======================================================================================
// Cube.h                                                                         
//=======================================================================================

#ifndef Cube_H
#define Cube_H

#include "Utility.h"

class Cube
{
public:
	Cube();
	~Cube();

	
 
	void init(ID3D10Device* device, float scale);
	void update(float dt);
	void draw();

private:
	DWORD mNumRows;
	DWORD mNumCols;

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;//Device
	ID3D10Buffer* mVB; // Vertex buffer
	ID3D10Buffer* mIB; // Index  buffer
};

#endif // Cube_H
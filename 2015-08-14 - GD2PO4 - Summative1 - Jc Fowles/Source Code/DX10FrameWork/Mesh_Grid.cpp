#include "Mesh_Grid.h"
#include "DX10Utilities.h"

CMesh_Grid::CMesh_Grid()
{
}

CMesh_Grid::~CMesh_Grid()
{
}

bool CMesh_Grid::Initialise(CDX10Renderer* _pRenderer, UINT _width, UINT _length, D3DXCOLOR _color)
{
	// Store the Render
	m_pRenderer = _pRenderer;
	
	// Create the Grid
	VALIDATE(CreateGrid(_width, _length, _color));
	
	return true;

}

void CMesh_Grid::Draw(UINT _vertexLayoutID, UINT _techniqueID)
{
	// Render the Object
	m_pRenderer->RenderObject(m_staticBufferID, _vertexLayoutID, _techniqueID);
}

bool CMesh_Grid::CreateGrid(UINT _width, UINT _length, D3DXCOLOR _color)
{
	// TO DO: Clean up redundant code

	// Calculate number of verts
	UINT NumVertices = (_width + 1) * (_length + 1);
	TVertexColor* gridVertices = new TVertexColor[NumVertices];
	UINT stride = sizeof(*gridVertices);
	memset(gridVertices, 0, sizeof(TVertexColor) * NumVertices);

	// TO DO: Num poiy - 1

	// Calculate number of Indices
	int numIndPerRow = _width * 2 + 2;
	int numIndDegensReq = (_length - 1) * 2;
	int numIndices = (NumVertices * 3) - 1; // numIndPerRow * _length + numIndDegensReq;


	UINT* gridIndices = new UINT[numIndices];
	memset(gridIndices, 0, sizeof(UINT) * numIndices);

	GenerateVertices(&gridVertices, _width, _length);
	GenerateIndices(&gridIndices, _width, _length);
	
	VALIDATE(m_pRenderer->CreateStaticBuffer(gridVertices, gridIndices, NumVertices, numIndices, stride, &m_staticBufferID));

	delete gridVertices;
	gridVertices = 0;

	delete gridIndices;
	gridIndices = 0;

	return true;
}

void CMesh_Grid::GenerateVertices(TVertexColor** ppVertices, UINT verticesAlongWidth, UINT verticesAlongLength)
{
	delete *ppVertices;

	*ppVertices = new TVertexColor[(verticesAlongLength +1) * (verticesAlongWidth+1)];

	float halfWidth = ((float)verticesAlongWidth - 1.0f) / 2.0f;
	float halfLength = ((float)verticesAlongLength - 1.0f) / 2.0f;

	for (UINT row = 0; row < verticesAlongLength; row++)
	{
		for (UINT col = 0; col < verticesAlongWidth; col++)
		{
			
			(*ppVertices)[row * verticesAlongLength + col] = TVertexColor{
				D3DXVECTOR3((float)col - halfWidth, 0.0f, (float)row - halfLength),
				Color::CYAN
			};
			
		
			if ((row == 0) || (col == 0))
			{
				(*ppVertices)[row * verticesAlongLength + col].pos.y = 0.0f;
			}
			else if ((row == (verticesAlongLength - 1)) || (col == (verticesAlongWidth - 1)))
			{
				(*ppVertices)[row * verticesAlongLength + col].pos.y = 0.0f;
			}
			else
			{
				// Randomize the Y value 
				(*ppVertices)[row * verticesAlongLength + col].pos.y = float((rand() % 10) );
			}


		}
	}
}

UINT CMesh_Grid::GenerateIndices(UINT** ppIndices, UINT verticesAlongWidth, UINT verticesAlongDepth)
{
	
	int index = 0;
	// Go through all rows except last one
	for (unsigned int row = 0; row < verticesAlongDepth - 1; row++)
	{
		if (row % 2 == 0)
			// Even Row
		{
			// Even Rows go towards the Left for Clockwise winding order
			for (int col = (int)(verticesAlongWidth - 1); col >= 0; col--)
			{
				
				(*ppIndices)[index++] = (col + (row * verticesAlongWidth));
				(*ppIndices)[index++] = (col + ((row + 1) * verticesAlongWidth));
			}

			// Add Degenerate triangle at end of each row
			if (row != verticesAlongDepth - 2)
			{
				(*ppIndices)[index++] = (0 + ((row + 1) * verticesAlongWidth));
			}
		}
		else
			// Odd Rows
		{
			// Odd Rows go towards the Right for Clockwise winding order
			for (unsigned int col = 0; col < verticesAlongWidth; col++)
			{
				(*ppIndices)[index++] = (col + (row * verticesAlongWidth));
				(*ppIndices)[index++] = (col + ((row + 1) * verticesAlongWidth));
			}

			// Add Degenerate triangle at end of each row
			if (row != verticesAlongDepth - 2)
			{
				(*ppIndices)[index++] = ((verticesAlongWidth - 1) + ((row + 1)  * verticesAlongWidth));
			}
		}
	}

	//Return the Number of Indices created
	return index;


}
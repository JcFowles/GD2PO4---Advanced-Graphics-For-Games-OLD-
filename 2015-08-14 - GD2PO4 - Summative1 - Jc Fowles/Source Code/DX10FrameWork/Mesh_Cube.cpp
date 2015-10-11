#include "Mesh_Cube.h"


CMesh_Cube::CMesh_Cube()
{
}


CMesh_Cube::~CMesh_Cube()
{
}

bool CMesh_Cube::Initialise(CDX10Renderer* _pRenderer, D3DXCOLOR _color)
{
	// Store the Render
	m_pRenderer = _pRenderer;

	float scale = 1.0f; 
	float vertSize = 1.0f * scale / 2;

	TVertexColor vertices[] =
	{
		// Front
		{ D3DXVECTOR3(-vertSize, vertSize, -vertSize), _color },
		{ D3DXVECTOR3(vertSize, vertSize, -vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, -vertSize), _color },
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize), _color },

		// Left
		{ D3DXVECTOR3(-vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(-vertSize, vertSize, -vertSize), _color },
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize), _color },
		{ D3DXVECTOR3(-vertSize, -vertSize, vertSize), _color },

		// Right
		{ D3DXVECTOR3(vertSize, vertSize, -vertSize), _color },
		{ D3DXVECTOR3(vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, -vertSize), _color },

		// Back
		{ D3DXVECTOR3(vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(-vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(-vertSize, -vertSize, vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, vertSize), _color },

		// Top
		{ D3DXVECTOR3(-vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(vertSize, vertSize, vertSize), _color },
		{ D3DXVECTOR3(vertSize, vertSize, -vertSize), _color },
		{ D3DXVECTOR3(-vertSize, vertSize, -vertSize), _color },

		// Bottom
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, -vertSize), _color },
		{ D3DXVECTOR3(vertSize, -vertSize, vertSize), _color },
		{ D3DXVECTOR3(-vertSize, -vertSize, vertSize), _color }
	};


	UINT stride = sizeof(*vertices);
	UINT vertexCount = (sizeof(vertices) / stride);

	
	// Set up the Indices
	DWORD indices[] =
	{
		// Front 
		0, 1, 2,
		0, 2, 3,

		// Left 
		4, 5, 6,
		4, 6, 7,

		// Right 
		8, 9, 10,
		8, 10, 11,

		// Back 
		12, 13, 14,
		12, 14, 15,

		// Top 
		16, 17, 18,
		16, 18, 19,

		// Bottom 
		20, 21, 22,
		20, 22, 23
	};

	UINT indexCount = (sizeof(indices) / sizeof(*indices));

	// Create the Static buffer
	VALIDATE(m_pRenderer->CreateStaticBuffer(vertices, indices, vertexCount, indexCount, stride, &m_staticBufferID));


	/*switch (_vertexType)
	{
	case VT_BASIC:
	{
		
	}
		break;
	case VT_COLOR:
	{

	}
		break;
	default:
	{

	}
		break;
	}*/
	
	// Return successful initialization 
	return true;
}

bool CMesh_Cube::Initialise(CDX10Renderer* _pRenderer)
{
	// Store the Render
	m_pRenderer = _pRenderer;

	float scale = 1.0f; 
	float vertSize = 1.0f * scale / 2;

	TVertexBasic vertices[] =
	{
		// Front
		{ D3DXVECTOR3(-vertSize,  vertSize, -vertSize)  },
		{ D3DXVECTOR3( vertSize,  vertSize, -vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize, -vertSize)  },
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize)  },

		// Left
		{ D3DXVECTOR3(-vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3(-vertSize,  vertSize, -vertSize)  },
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize)  },
		{ D3DXVECTOR3(-vertSize, -vertSize,  vertSize)  },

		// Right
		{ D3DXVECTOR3( vertSize,  vertSize, -vertSize)  },
		{ D3DXVECTOR3( vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize,  vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize, -vertSize)  },

		// Back
		{ D3DXVECTOR3( vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3(-vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3(-vertSize, -vertSize,  vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize,  vertSize)  },

		// Top
		{ D3DXVECTOR3(-vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3( vertSize,  vertSize,  vertSize)  },
		{ D3DXVECTOR3( vertSize,  vertSize, -vertSize)  },
		{ D3DXVECTOR3(-vertSize,  vertSize, -vertSize)  },

		// Bottom
		{ D3DXVECTOR3(-vertSize, -vertSize, -vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize, -vertSize)  },
		{ D3DXVECTOR3( vertSize, -vertSize,  vertSize)  },
		{ D3DXVECTOR3(-vertSize, -vertSize,  vertSize)  }
	};


	UINT stride = sizeof(*vertices);
	UINT vertexCount = (sizeof(vertices) / stride);

	DWORD indices[] =
	{
		// Front 
		0, 1, 2,
		0, 2, 3,

		// Left 
		4, 5, 6,
		4, 6, 7,

		// Right 
		8, 9, 10,
		8, 10, 11,

		// Back 
		12, 13, 14,
		12, 14, 15,

		// Top 
		16, 17, 18,
		16, 18, 19,

		// Bottom 
		20, 21, 22,
		20, 22, 23
	};

	UINT indexCount = (sizeof(indices) / sizeof(*indices));

	// Create the Static buffer
	VALIDATE(m_pRenderer->CreateStaticBuffer(vertices, indices, vertexCount, indexCount, stride, &m_staticBufferID));

	// Return successful initialization 
	return true;
}

void CMesh_Cube::Draw(UINT _vertexLayoutID, UINT _techniqueID)
{
	// Render the Object
	m_pRenderer->RenderObject(m_staticBufferID, _vertexLayoutID, _techniqueID);
}

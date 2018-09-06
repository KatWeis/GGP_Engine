#pragma once

#include <d3d11.h>
#include "Vertex.h"

// --------------------------------------------------------
// A custom mesh definition
//
// --------------------------------------------------------
class Mesh
{
public:
	Mesh(Vertex* vertices, int numVerts, unsigned int* indices, int numIndices, ID3D11Device* device);
	~Mesh();

	//Get accessors that allow Meshes to retrieve the pointer to their buffers so they can be drawn
	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer; }
	int GetIndexCount() { return indexCount; }
private:
	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Number of indices in the mesh's index buffer
	int indexCount;
};


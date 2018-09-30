#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include <string>
#include <vector>
#include <fstream>

using namespace DirectX;

// --------------------------------------------------------
// A custom mesh definition
//
// --------------------------------------------------------
class Mesh
{
public:
	Mesh(Vertex* vertices, unsigned int numVerts, unsigned int* indices, unsigned int numIndices, ID3D11Device* device);
	Mesh(char* filename, ID3D11Device * device);
	~Mesh();

	// Get accessors that allow Meshes to retrieve the pointer to their buffers so they can be drawn
	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer; }
	int GetIndexCount() { return indexCount; }
private:
	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Number of indices in the mesh's index buffer
	int indexCount;

	// Helper method that creates the index and vertex buffers
	void CreateBuffers(Vertex* vertices, unsigned int numVerts, unsigned int* indices, unsigned int numIndices, ID3D11Device* device);
};


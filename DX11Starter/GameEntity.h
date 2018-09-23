#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class GameEntity
{
public:
	GameEntity(Mesh* m, Material* mat);
	~GameEntity();

	// Moves the Entity by the specified amount along the axes
	void Move(float x, float y, float z);
	// Scales the Entity by the amount specified
	// Do NOT pass in negatives to scale down, use decimals
	void Scale(float x, float y, float z);
	// Rotates the Entity by the amount specified around the axes
	void Rotate(float x, float y, float z);

	// Calculate the World Matrix
	// This should be called once per frame, before draw
	void CalculateWorldMatrix();

	// Set up the material and shaders to draw the entity correctly
	void PrepareMaterial(XMFLOAT4X4 viewMatix, XMFLOAT4X4 projMatrix);

	// Accessors to retrieve important info about the Entity
	XMFLOAT4X4 GetWorldMatrix() { return worldMatrix; };
	Mesh* GetMesh() { return mesh; };
	XMFLOAT3 GetPosition() { return position; };
	XMFLOAT3 GetRotation() { return rotation; };
	XMFLOAT3 GetScale() { return scale; };

private:
	// World Matrix for transforming the Game Entity
	XMFLOAT4X4 worldMatrix;

	// Vectors for creating the World Matrix
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	// Vectors of the transforms from last frame 
	XMFLOAT3 lastPosition;
	XMFLOAT3 lastRotation;
	XMFLOAT3 lastScale;

	// Pointer to the Mesh used by this Game Entity
	Mesh* mesh;

	// Pointer to the Material used by this Game Entity
	Material* material;

	// Determines if the World Matrix is dirty and needs to be recalculated
	// Checks if the position, rotation, or scale of the Entity has changed since last frame
	bool IsWorldMatrixDirty();

};


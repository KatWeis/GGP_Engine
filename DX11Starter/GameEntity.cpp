#include "GameEntity.h"

GameEntity::GameEntity(Mesh* m)
{
	mesh = m;

	// Store the identity matrix values in the worldMatrix variable
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());

	// Store default values in the pos, rot, and scale
	position.x = 0;
	position.y = 0;
	position.z = 0;

	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;

	scale.x = 1;
	scale.y = 1;
	scale.z = 1;
}


GameEntity::~GameEntity()
{
	// Nothing to clean up right now, may change in future
}

void GameEntity::Move(float x, float y, float z)
{
	position.x = position.x + x;
	position.y = position.y + y; 
	position.z = position.z + z;
}

void GameEntity::Scale(float x, float y, float z)
{
	scale.x = scale.x * x;
	scale.y = scale.y * y;
	scale.z = scale.z * z;
}

void GameEntity::Rotate(float x, float y, float z)
{
	rotation.x = rotation.x + x;
	rotation.y = rotation.y + y;
	rotation.z = rotation.z + z;
}

void GameEntity::CalculateWorldMatrix()
{
	if (!IsWorldMatrixDirty())
		return;

	// Load the member variables over into temp types for calculations
	XMMATRIX worldMat = XMLoadFloat4x4(&worldMatrix);
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR rot = XMLoadFloat3(&rotation);
	XMVECTOR scl = XMLoadFloat3(&scale);

	// Calculate the new World Matrix
	XMMATRIX translationMat = XMMatrixTranslationFromVector(pos);
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(rot);
	XMMATRIX scaleMat = XMMatrixScalingFromVector(scl);

	worldMat = XMMatrixIdentity() * translationMat * rotationMat * scaleMat;

	// Remember to transpose it at the end!
	// Store result
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));

	// Update variables to be accurate to change
	lastPosition = position;
	lastRotation = rotation;
	lastScale = scale;
}

bool GameEntity::IsWorldMatrixDirty()
{
	if (lastPosition.x == position.x && lastPosition.y == position.y && lastPosition.z == position.z
		&& lastRotation.x == rotation.x && lastRotation.y == rotation.y && lastRotation.z == rotation.z
		&& lastScale.x == scale.x && lastScale.y == scale.y && lastScale.z == scale.z) {
		return false;
	}
	return true;
}

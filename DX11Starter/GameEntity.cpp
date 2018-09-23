#include "GameEntity.h"

GameEntity::GameEntity(Mesh* m, Material* mat)
{
	mesh = m;
	material = mat;

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

void GameEntity::PrepareMaterial(XMFLOAT4X4 viewMatix, XMFLOAT4X4 projMatrix)
{
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	material->GetVertexShader()->SetMatrix4x4("world", worldMatrix);
	material->GetVertexShader()->SetMatrix4x4("view", viewMatix);
	material->GetVertexShader()->SetMatrix4x4("projection", projMatrix);

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	material->GetVertexShader()->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();
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

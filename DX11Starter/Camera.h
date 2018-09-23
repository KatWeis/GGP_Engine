#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	// Updates the view matrix 
	void Update(float deltaTime);

	// Calculate the new projection matrix
	void UpdateProjectionMatrix(unsigned int width, unsigned int height);

	// Rotates the Camera along the X and Y axes based on Mouse movement
	void MouseLook(float rotX, float rotY);

	// Accessors to retrieve important info about the Camera
	XMFLOAT4X4 GetViewMatrix() { return viewMatrix; };
	XMFLOAT4X4 GetProjectionMatrix() { return projMatrix; };

private:
	// View Matrix for transforming the Camera and determining what is in the Camera's view
	XMFLOAT4X4 viewMatrix;

	// Projection Matrix for transforming the Camera view to a 2D representation
	XMFLOAT4X4 projMatrix;

	// Vectors for creating the View Matrix
	XMFLOAT3 position;
	XMFLOAT3 forward;

	// Floats for determing the Camera's rotation around each axis
	float rotationX;
	float rotationY;

	// Vectors and Floats of the transforms from last frame 
	XMFLOAT3 lastPosition;
	float lastRotationX;
	float lastRotationY;

	// Determines if the View Matrix is dirty and needs to be recalculated
	// Checks if the position or rotation on any axis of the Camera has changed since last frame
	bool IsViewMatrixDirty();

	// Checks for keyboard input and moves Camera appropriately 
	// W, S - Move the Camera back and forth along its forward vector
	// A, D – Strafe left or right (based on your current rotation)
	// Spacebar - Move up along the world’s Y axis (regardless of your rotation)
	// X - Move down along the world’s Y axis (regardless of your rotation)
	void ProcessInput(float deltaTime);
};


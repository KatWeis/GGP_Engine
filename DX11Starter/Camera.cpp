#include "Camera.h"



Camera::Camera()
{
	// Store default values in the pos, rot, and forward
	position.x = +0.0f;
	position.y = +0.0f;
	position.z = -5.0f;

	forward.x = +0.0f;
	forward.y = +0.0f;
	forward.z = +1.0f;

	rotationX = +0.0f;
	rotationY = +0.0f;

	speed = 5;
}


Camera::~Camera()
{
	// Nothing to clean up right now, may change in future
}

void Camera::Update(float deltaTime)
{
	ProcessInput(deltaTime);

	if (!IsViewMatrixDirty()) {
		return;
	}

	// Load the member variables over into temp types for calculations
	XMVECTOR pos = XMLoadFloat3(&position);

	// Recalculate the forward vector of the Camera
	XMVECTOR dir = XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMQuaternionRotationRollPitchYaw(rotationX, rotationY, 0.0f));
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Calculate a new View Matrix
	XMMATRIX viewMat = XMMatrixLookToLH(pos, dir, up);

	// Remember to transpose it at the end!
	// Store result
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(viewMat));
	XMStoreFloat3(&forward, dir);

	// Update variables to be accurate to change
	lastPosition = position;
	lastRotationX = rotationX;
	lastRotationY = rotationY;
}

void Camera::UpdateProjectionMatrix(unsigned int width, unsigned int height)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::MouseLook(float rotX, float rotY)
{
	rotationX += rotX*.001f;
	rotationY += rotY*.001f;

	// If Y Rotation exceeds 360 degrees, reset it so we aren't dealing with massive numbers
	if (rotationY > XM_2PI) {
		rotationY -= XM_2PI;
	}
	if (rotationY < -XM_2PI) {
		rotationY += XM_2PI;
	}

	// Limit rotation on the X axis (Pitch) to only allow range betweem straight up and straight down
	if (rotationX > XM_PIDIV4) {
		rotationX = XM_PIDIV4;
	}
	if (rotationX < -XM_PIDIV4) {
		rotationX = -XM_PIDIV4;
	}
}

bool Camera::IsViewMatrixDirty()
{
	if (lastPosition.x == position.x && lastPosition.y == position.y && lastPosition.z == position.z
		&& lastRotationX == rotationX && lastRotationY == rotationY) {
		return false;
	}
	return true;
}

void Camera::ProcessInput(float deltaTime)
{
	// Load the member variables over into temp types for calculations
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&forward);
	XMVECTOR right = XMVector3Cross(dir, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	if (GetAsyncKeyState('W') & 0x8000) {
		pos += dir * speed * deltaTime;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		pos += (-1)* dir * speed * deltaTime;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		pos += right * speed * deltaTime;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		pos += (-1) * right * speed * deltaTime;
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		pos -= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * speed * deltaTime;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		pos += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * speed * deltaTime;
	}

	// Store result
	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&forward, dir);
}

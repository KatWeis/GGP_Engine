#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Lights.h"
#include <DirectXMath.h>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// First Person Debug Camera
	Camera* mainCamera;

	// Lights
	DirectionalLight dirLight;
	DirectionalLight light2;

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;

	// Meshes to draw to the screen
	Mesh* triangle;
	Mesh* trapezoid;
	Mesh* square;
	Mesh* cone;
	Mesh* sphere;
	Mesh* helix;

	// Materials to assign to GameEntities
	Material* ice;
	Material* cobble;
	Material* tiles;

	// Vector of GameEntities in the Game
	std::vector<GameEntity>* entities;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	// Keeps track of the position the mouse had when the button was initially clicked
	POINT downMousePos;
};


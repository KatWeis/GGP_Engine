#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;

	mainCamera = new Camera();

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Delete our Meshes, which will clean up their own buffers
	delete triangle;
	delete trapezoid;
	delete square;
	delete cone;
	delete sphere;
	delete helix;

	// Delete our Materials
	delete cobble;
	delete ice;
	delete tiles;

	// Delete the game entities, they will clean up themselves
	delete entities;

	// Delete the Camera
	delete mainCamera;

	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vertexBuffer) { vertexBuffer->Release(); }

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

	// Set the projection matrix of the main camera
	mainCamera->UpdateProjectionMatrix(width, height);

	// Initialize the lights in the scene
	dirLight = { XMFLOAT4(+0.1f, +0.1f, +0.1f, +1.0f), XMFLOAT4(+0.0f, +1.0f, +0.0f, +1.0f), XMFLOAT3(+1.0f, -1.0f, +0.0f) };
	light2 = { XMFLOAT4(+0.1f, +0.1f, +0.1f, +1.0f), XMFLOAT4(+0.75f, +0.5f, +0.0f, +1.0f), XMFLOAT3(+0.0f, +1.0f, +1.0f) };

	// Pass the light to the pixel shader
	pixelShader->SetData(
		"light",  // The name of the (eventual) variable in the shader
		&dirLight,   // The address of the data to copy
		sizeof(DirectionalLight)); // The size of the data to copy

	// Pass the light to the pixel shader
	pixelShader->SetData(
		"light2",  // The name of the (eventual) variable in the shader
		&light2,   // The address of the data to copy
		sizeof(DirectionalLight)); // The size of the data to copy

	// Load a texture
	ID3D11ShaderResourceView* srvIce;
	CreateWICTextureFromFile(device, context, L"./Assets/Textures/ice.jpg", 0, &srvIce);

	ID3D11ShaderResourceView* srvCobble;
	CreateWICTextureFromFile(device, context, L"./Assets/Textures/cobble.jpg", 0, &srvCobble);

	ID3D11ShaderResourceView* srvTiles;
	CreateWICTextureFromFile(device, context, L"./Assets/Textures/tiles_med.tif", 0, &srvTiles);

	// Create a SamplerState
	ID3D11SamplerState* sample;
	// Create the SAMPLER STATE description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_SAMPLER_DESC sd;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Tells DirectX how to handle UV coordinates outside of the 0 - 1 range
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MinLOD = 0;
	sd.MipLODBias = 0;
	//sd.BorderColor = 0;
	sd.MaxAnisotropy = 0;
	//sd.ComparisonFunc = 0;

	device->CreateSamplerState(&sd, &sample);

	// Create a basic Material
	ice = new Material(vertexShader, pixelShader, srvIce, sample);
	cobble = new Material(vertexShader, pixelShader, srvCobble, sample);
	tiles = new Material(vertexShader, pixelShader, srvTiles, sample);

	// Create and add some entities to the game
	entities = new std::vector<GameEntity>();
	entities->push_back(GameEntity(cone, ice));
	entities->push_back(GameEntity(helix, tiles));
	entities->push_back(GameEntity(sphere, cobble));

	(*entities)[0].Move(3, 0, 0);
	(*entities)[2].Move(-3, 0, 0);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent uvs and normals
	// - Not necessary, just makes things more readable
	XMFLOAT2 uv = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT3 normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), normal, uv },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	// Create a triangle Mesh using the vertices and indices specified earlier
	triangle = new Mesh(vertices, 3, indices, 3, device);

	//set vertices and indices for next Mesh
	Vertex verticesTrap[] =
	{
		{ XMFLOAT3(+2.0f, -0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(+2.5f, -0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(+3.0f, +0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(+1.5f, +0.5f, +0.0f), normal, uv },
	};

	unsigned int indicesTrap[] = { 2, 1, 0, 0, 3, 2 };

	// Create a trapezoid Mesh using the vertices and indices specified earlier
	trapezoid = new Mesh(verticesTrap, 4, indicesTrap, 6, device);

	//set vertices and indices for next Mesh
	Vertex verticesSq[] =
	{
		{ XMFLOAT3(-3.5f, -0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(-2.0f, -0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(-2.0f, +0.5f, +0.0f), normal, uv },
		{ XMFLOAT3(-3.5f, +0.5f, +0.0f), normal, uv },
	};

	unsigned int indicesSq[] = { 2, 1, 0, 0, 3, 2 };

	// Create a square Mesh using the vertices and indices specified earlier
	square = new Mesh(verticesSq, 4, indicesSq, 6, device);

	// Create meshes from the data in obj files
	cone = new Mesh("./Assets/Models/cone.obj", device);
	helix = new Mesh("./Assets/Models/helix.obj", device);
	sphere = new Mesh("./Assets/Models/sphere.obj", device);
	
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	mainCamera->UpdateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// Update the Camera
	mainCamera->Update(deltaTime);

	// Move some of the Game Entities around
	(*entities)[1].Rotate(0, 1.0f * deltaTime, 0.0f);//rotates helix around y axis counterclockwise
	(*entities)[2].Rotate(-0.25f * deltaTime, 0, 0);//rotates sphere around x axis clockwise
	(*entities)[0].Move(0, sin(totalTime) * deltaTime, 0);//moves cone in sine curve along y axis
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	for (int i = 0; i < entities->size(); i++) {

		// Calculate the world matrix of the entity
		(*entities)[i].CalculateWorldMatrix();

		(*entities)[i].PrepareMaterial(mainCamera->GetViewMatrix(), mainCamera->GetProjectionMatrix());

		// Set buffers in the input assembler
		//  - Do this ONCE PER OBJECT you're drawing, since each object might
		//    have different geometry.
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		vertexBuffer = (*entities)[i].GetMesh()->GetVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer((*entities)[i].GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			(*entities)[i].GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices

		// Set the buffers back to defaults so they aren't constantly deleted
		vertexBuffer = 0;
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Save the mouse position when the button is clicked for the future
	downMousePos.x = x;
	downMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	SetCursorPos(prevMousePos.x, prevMousePos.y);

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Rotate Camera
	if (buttonState & 0x0001) {
		mainCamera->MouseLook(prevMousePos.y - y, prevMousePos.x - x);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion
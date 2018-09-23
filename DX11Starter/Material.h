#pragma once
#include "DXCore.h"
#include "SimpleShader.h"

class Material
{
public:
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader);
	~Material();

	// Accessors to retrieve important info about the Material
	SimpleVertexShader* GetVertexShader() { return vertexShader; };
	SimplePixelShader* GetPixelShader() { return pixelShader; };

private:
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
};


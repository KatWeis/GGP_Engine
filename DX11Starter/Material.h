#pragma once
#include "DXCore.h"
#include "SimpleShader.h"

class Material
{
public:
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* srvPtr, ID3D11SamplerState* sampleState);
	~Material();

	// Accessors to retrieve important info about the Material
	SimpleVertexShader* GetVertexShader() { return vertexShader; };
	SimplePixelShader* GetPixelShader() { return pixelShader; };
	ID3D11ShaderResourceView* GetSRV() { return srv; };
	ID3D11SamplerState* GetSamplerState() { return sample; };

private:
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// Pointers to variables needed to keep track of the material's texture
	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* sample;
};


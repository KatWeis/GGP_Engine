#include "Material.h"



Material::Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* srvPtr, ID3D11SamplerState* sampleState)
{
	vertexShader = vShader;
	pixelShader = pShader;

	srv = srvPtr;
	sample = sampleState;
}


Material::~Material()
{
}

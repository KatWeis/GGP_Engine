#pragma once

#include <DirectXMath.h>
// --------------------------------------------------------
// A custom directional light definition
// --------------------------------------------------------
struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColor;	    // The color of the ambient lighting
	DirectX::XMFLOAT4 DiffuseColor;     // The color of the diffuse lighting
	DirectX::XMFLOAT3 Direction;        // The direction of the light
};
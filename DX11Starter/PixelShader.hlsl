
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv		    : TEXCOORD;
};

// --------------------------------------------------------
// A custom directional light definition
// --------------------------------------------------------
struct DirectionalLight
{
	float4 ambient;	    // The color of the ambient lighting
	float4 diffuse;     // The color of the diffuse lighting
	float3 direction;   // The direction of the light
};

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
};

Texture2D diffuseTexture  : register(t0); 
SamplerState basicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Normalize the normal to ensure its a unit vector
	input.normal = normalize(input.normal);

	// Calculate the unit vector of the direction to the light
	float3 dirToLight = normalize((-1)*light.direction);

	// Calculate the amount of diffuse lighting on the current pixel
	float lightAmt = saturate(dot(input.normal, dirToLight));

	// Do the same for the second directional light
	float3 dirToLight2 = normalize((-1)*light2.direction);
	float lightAmt2 = saturate(dot(input.normal, dirToLight2));

	// Calculate the surface color of this pixel based on the texture at this uv coordinate
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return (light.diffuse * lightAmt)*surfaceColor + (light2.diffuse * lightAmt2)*surfaceColor + light.ambient*surfaceColor;
}
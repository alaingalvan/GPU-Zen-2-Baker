#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

precision mediump float;
precision highp int;

/**************************************************************
* Uniforms
**************************************************************/

layout(std140) uniform;

uniform FragUBO
{
  vec3 cameraPosition;
} ubo;


/**************************************************************
* Varying
**************************************************************/

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormals;
layout(location = 2) in vec3 inTangents;
layout(location = 3) in vec3 inBitangents;
layout(location = 4) in vec4 inColors;
layout(location = 5) in vec2 inTextureCoords;
layout(location = 6) in vec3 inBakeDir;


/**************************************************************
* Outputs
**************************************************************/

layout (location = 0) out vec4 outHeight;

/**************************************************************
* Functions
**************************************************************/

vec3 findRayDirection(vec3 position, vec3 smoothNormal, 
                           vec2 uv, sampler2D dirMask)
{
	vec3 dx = dFdx(inPosition);
	vec3 dy = dFdy(inPosition);
	vec3 faceNormal = cross(dx, dy);

	float traceBlend = textureLod(dirMask, uv, 0.0).x;

	if( dot(faceNormal, smoothNormal ) < 0.0 )
	{ faceNormal = -faceNormal; }
	
	vec3 diff = smoothNormal - faceNormal;
	float diffLen = length( diff );
	float maxLen = 1.414 * traceBlend;
	if( diffLen > maxLen )
	{ diff *= maxLen/diffLen; }
	vec3 dir = faceNormal + diff;

	return -normalize(dir);
}

/**************************************************************
* Main
**************************************************************/

void main()
{
	vec3 traceDir = findRayDirection(inPosition, normalize(inBakeDir), inTextureCoords, dirMask);
	vec3 tracePos = findTraceOrigin(inPosition, traceDir, inTextureCoords, offMask);

	TriangleHit hit;
	bool didhit = traceRay(tracePos, traceDir, hit);

	float height = didhit ? hit.distance : 0.0;
	height -= length(inPosition - tracePos);
	height = height * uHeightScaleBias.x + uHeightScaleBias.y;

	outHeight.xyz = vec3(height, height, height);
	outHeight.w = didhit ? 1.0 : 0.0;
}

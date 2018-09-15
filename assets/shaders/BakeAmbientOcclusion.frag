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

layout (location = 0) out vec4 outAO;

/**************************************************************
* Functions
**************************************************************/

float rand( float seed )
{
	return abs( fract( sin( seed ) * 43758.5453423 ) );
}

vec3 rand3( float seed )
{
	float theta = 2.0 * 3.142 * rand( seed );
	float y = rand( seed + 31.0 ) * 2.0 - 1.0;
	float a = sqrt( 1.0 - y * y );
	return vec3( a * cos( theta ) , y, a * sin( theta ) );
}

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
	outAO = vec4(1.0,1.0,1.0,0.0);

	vec3 traceDir = findRayDirection( inPosition, normalize(inBakeDir), inTextureCoords, dirMask);
	vec3 tracePos = findTraceOrigin( inPosition, traceDir, inTextureCoords, offMask );

	TriangleHit hit;
	HINT_BRANCH
	if (traceRay( tracePos, traceDir, hit))
	{
		vec3 pos = tracePos + traceDir * (hit.distance - uHemisphereOffset);

		vec3 basisY =	hit.coords.x * uNormals[hit.vertices.x] +
						hit.coords.y * uNormals[hit.vertices.y] +
						hit.coords.z * uNormals[hit.vertices.z];

		basisY = normalize( basisY );		

		vec3 basisX = normalize( cross( basisY, fTangent ) );

		vec3 basisZ = cross( basisX, basisY );

		// We'll only use 1 sample here:
		float ao = 0.0;

    // Random Direction in hemisphere of first hit
    vec3 d = rand3( fTexCoord + uRandSeed );
    d = normalize(d);

    float dw = d.y;
    d = d.x * basisX + d.y * basisY + d.z * basisZ;

		TriangleHit hit2;
    if( traceRay2( pos, d, hit2 ) )
    {
      ao += dw;
    }

    ao = 1.0 - ao;
  }

  outAO.xyz = vec3(ao, ao, ao);
  outAO.w = 1.0;
}


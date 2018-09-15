#pragma once

#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "Utils.h"

#include "CrossWindow/CrossWindow.h"
#include "glad/glad.h"
#include "CrossWindow/Graphics.h"
#include "vectormath.hpp"

#include "Scene.h"

class Renderer
{
public:
	Renderer();

	struct RenderTarget
	{
		GLuint framebuffer;
		GLuint attachments[8];
		GLuint depthstencil;
		unsigned width;
		unsigned height;

		RenderTarget() : framebuffer(0), attachments{ 0, 0, 0, 0, 0, 0, 0, 0 }, depthstencil(-1), width(1), height(1)
		{}

		void create();

	protected:
		bool mCreated = false;
	};

	struct ResizeDesc
	{
		unsigned width;
		unsigned height;
		RenderTarget* target;
		
		ResizeDesc() : width(1), height(1), target(nullptr)
		{}
	};

	bool resize(const ResizeDesc desc);

	bool initializeAPI();

	void setContext(xgfx::OpenGLState state);

	void setRenderTarget(RenderTarget* target);

	void traverse(ActorPtr actor, float deltaTime);

	// Render the scene onto the current target
	void render(Scene& scene);

	// Returns the time between frames (in seconds)
	float getDeltaTime();

protected:

	// Clock
	std::chrono::time_point<std::chrono::steady_clock> tStart, tEnd;
	float mDeltaTime;
	float mFrameRate;

	// Main Camera Data
	struct {
		Matrix4 viewMatrix;
		Matrix4 projectionMatrix;
	} mCameraUniformData;

	GLuint mCameraUniformBuffer;

	// Final Output FrameBufer
	RenderTarget* mRenderTarget;
	xgfx::OpenGLState mOpenGLState;

	// Default screen covering triangle
	GLuint mVertexArray;
	GLuint mVertexBuffer;
	float mVertexBufferData[12] =
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

};
#include "Renderer.h"

Renderer::Renderer()
{
	mDeltaTime = 0.0f;
	mFrameRate = 60.0f;
}

bool Renderer::initializeAPI()
{
	if (!gladLoadGL())
	{
		// Failed
		std::cout << "Failed to load OpenGL.";
		return false;
	}
#if defined(_DEBUG)
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL started with error code: " << err;
		return false;
	}
#endif

	// Setup local data structures

	// 🎥 Camera UBO
	glGenBuffers(1, &mCameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mCameraUniformData), &mCameraUniformData, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, mCameraUniformBuffer, 0, sizeof(mCameraUniformData));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// 🔼 Default screen covering triangle
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, mVertexBufferData, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	// 🕘 Initialize Clock
	tStart = std::chrono::high_resolution_clock::now();

	return true;
}

void Renderer::setContext(xgfx::OpenGLState state)
{
	xgfx::setContext(state);
	mOpenGLState = state;
}

void Renderer::setRenderTarget(RenderTarget* target)
{
	mRenderTarget = target;
}

bool Renderer::resize(const ResizeDesc desc)
{
	desc.target->width = clamp(desc.width, 1u, 0xffffu);
	desc.target->height = clamp(desc.height, 1u, 0xffffu);

	// 🎥 Update main camera uniforms
	mCameraUniformData.projectionMatrix = Matrix4::perspective(45.0f, static_cast<float>(desc.target->width) / static_cast<float>(desc.target->height), 0.01f, 1024.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &mCameraUniformData, sizeof(mCameraUniformData));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// 🖼️ Recreate frame buffer
	desc.target->create();

	return true;
}

void Renderer::traverse(ActorPtr actor, float deltaTime)
{
	actor->update(deltaTime);

	// Draw actor through standard material pipeline

	for (ActorPtr& child : actor->getChildren())
	{
		traverse(child, deltaTime);
	}
}

void Renderer::render(Scene& scene)
{
	// ⏱️ Framelimit set to 60 fps
	tEnd = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
	if (time < (1000.0f / mFrameRate))
	{
		return;
	}
	tStart = std::chrono::high_resolution_clock::now();
	
	// 🕘 Update Clock
	mDeltaTime = 0.001f * time;

	// 🎥 Update main camera UBO
	glBindBuffer(GL_UNIFORM_BUFFER, mCameraUniformBuffer);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &mCameraUniformData, sizeof(mCameraUniformData));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// 🎨 Draw
	glBindFramebuffer(GL_FRAMEBUFFER, mRenderTarget->framebuffer);
	glViewport(0, 0, mRenderTarget->width, mRenderTarget->height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 🌳 Preorder traversal of scene hierarchy
	std::vector<ActorPtr>& sceneVec = scene.getVector();

	for (ActorPtr& actor : sceneVec)
	{
		traverse(actor, mDeltaTime);
	}

	// 💌 Blit framebuffer to window
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mRenderTarget->framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, mRenderTarget->width, mRenderTarget->height);
	glBlitFramebuffer(0, 0, mRenderTarget->width, mRenderTarget->height, 0, 0, mRenderTarget->width, mRenderTarget->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

float Renderer::getDeltaTime()
{
	return mDeltaTime;
}

void Renderer::RenderTarget::create()
{
	if(mCreated)
	{
		glDeleteTextures(1, &attachments[0]);
		glDeleteRenderbuffers(1, &depthstencil);
		glDeleteFramebuffers(1, &framebuffer);
	}

	mCreated = true;

	glGenTextures(1, &attachments[0]);
	glBindTexture(GL_TEXTURE_2D, attachments[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &depthstencil);
	glBindRenderbuffer(GL_RENDERBUFFER, depthstencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, attachments[0], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthstencil);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame Buffer Failed to be Created!";
	}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

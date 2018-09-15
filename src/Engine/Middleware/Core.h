#pragma once

#include "../Core/IMiddleware.h"
#include "../Core/Renderer.h"

#include "../Core/GUI.h"

#include "CrossWindow/Graphics.h"
#include "CrossWindow/CrossWindow.h"

/**
 * Core middleware to the engine which handles the windowing system and scene renderer.
 */
class CoreMiddleware : public IMiddleware
{

public:
	CoreMiddleware();

	virtual ~CoreMiddleware() override;

	virtual bool create(const EngineDesc& desc, const std::vector<IMiddlewarePtr>& middleware) override;

	virtual bool shouldUpdate() override;

	virtual void update(Engine* engine) override;

	// A coupling of the OS window with Graphics API state
	class GraphicWindow
	{
	public:
		// OS Window
		xwin::WindowPtr window;

		// Graphics API state
		xgfx::OpenGLState openGLState;

		// Frame Buffer Data
		Renderer::RenderTarget renderTarget;

		// Flag to enable / disable rendering
		bool shouldRender;
	};

	// 🖼️ Graphic Window
	GraphicWindow graphicWindow;

	// Scene renderer
	Renderer renderer;

protected:
	xwin::EventQueue mEventQueue;
	GUI mGui;

};

typedef std::shared_ptr<CoreMiddleware> CoreMiddlewarePtr;

// 🏭 Factory Function:
CoreMiddlewarePtr coreMiddleware();
#include "CoreMiddleware.h"
#include "../Engine.h"

CoreMiddleware::CoreMiddleware() : IMiddleware()
{}

CoreMiddleware::~CoreMiddleware()
{}

bool CoreMiddleware::create(const EngineDesc& desc, const std::vector<IMiddlewarePtr>& middleware)
{
	// 🌟 Create a single GraphicWindow
	graphicWindow.shouldRender = true;

	xwin::WindowDesc wdesc;
	wdesc.name = "MainWindow";
	wdesc.title = "GPU Zen 2 | Baking Textures Example";
	wdesc.visible = true;
	wdesc.width = 1280;
	wdesc.height = 720;
	wdesc.fullscreen = desc.fullscreen;

	graphicWindow.window = xwin::WindowPtr(new xwin::Window());
	graphicWindow.window->create(wdesc, mEventQueue);

	xgfx::OpenGLDesc ogldesc;
	graphicWindow.openGLState = xgfx::createContext(graphicWindow.window.get(), ogldesc);

	xgfx::setContext(graphicWindow.openGLState);

	// ✏️ Initialize Renderer
	if (!renderer.initializeAPI())
	{
		return false;
	}

	// Initialize Render Target
	RenderTargetDesc rtdesc;
	rtdesc.width = wdesc.width;
	rtdesc.height = wdesc.height;
	graphicWindow.renderTarget.create(rtdesc);

	// 👓 Initialize the IMGUI system
	GUIDesc guiDesc;
	guiDesc.width = wdesc.width;
	guiDesc.height = wdesc.height;
	if (!mGui.create(guiDesc))
	{
		return false;
	}

	return true;
}

bool CoreMiddleware::shouldUpdate()
{
	return true;
}

void CoreMiddleware::update(Engine* engine)
{
	// ♻️ Update the event queue
	mEventQueue.update();

	// 🎈 Iterate through that queue:
	while (!mEventQueue.empty())
	{
		const xwin::Event& e = mEventQueue.front();

		// 💗 On Resize:
		if (e.type == xwin::EventType::Resize)
		{
			const xwin::ResizeData data = e.data.resize;

			graphicWindow.shouldRender = false;

			Renderer::ResizeDesc resizeDesc;
			resizeDesc.width = data.width;
			resizeDesc.height = data.height;
			resizeDesc.target = &graphicWindow.renderTarget;
			renderer.resize(resizeDesc);
		}

		// ❌ On Close:
		if (e.type == xwin::EventType::Close)
		{
			e.window->close();
			graphicWindow.window = nullptr;
		}

		// 👓 Pass event to GUI system
		mGui.updateEvent(e);

		mEventQueue.pop();
	}

	// 🛑 Stop the engine if all windows are gone
	if (graphicWindow.window == nullptr)
	{
		engine->stop();
		return;
	}

	// ✨ Update Visuals
	if (graphicWindow.shouldRender)
	{
		// Set the target framebuffer
		renderer.setRenderTarget(&graphicWindow.renderTarget);

		// Render the scene to that framebuffer
		renderer.render(engine->getScene());

		// Render the GUI of that window
		mGui.update(renderer.getDeltaTime());

		// 🎞️ Swap BackBuffers
		xgfx::swapBuffers(graphicWindow.openGLState);
	}

	graphicWindow.shouldRender = true;

}

CoreMiddlewarePtr coreMiddleware()
{
	return CoreMiddlewarePtr(new CoreMiddleware());
}
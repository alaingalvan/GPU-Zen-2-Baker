#pragma once

#include "Core/EngineDesc.h"
#include "Core/IMiddleware.h"
#include "Core/Scene.h"

#include "Middleware/CoreMiddleware.h"

#include "Actors/Baker.h"

class Engine
{
public:
	Engine();

	~Engine();

	// Start the engine
	void start(const EngineDesc& desc);

	// Stop the engine
	void stop();

	Scene& getScene();

	const std::vector<IMiddlewarePtr>& getMiddleware();

protected:

	// If the engine is running or not
	bool mRunning;

	// a list of middleware managed by the engine
	std::vector<IMiddlewarePtr> mMiddleware;

	// A single actor scene hierarchy
	Scene mScene;
};

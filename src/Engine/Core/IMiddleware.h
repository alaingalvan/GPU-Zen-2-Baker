#pragma once

#include <memory>
#include <vector>

#include "EngineDesc.h"


class Engine;

class IMiddleware;

typedef std::shared_ptr<IMiddleware> IMiddlewarePtr;

class IMiddleware
{
public:
	IMiddleware();

	virtual ~IMiddleware();

	// a lifetime hook called by the engine when creating this middleware, used for setup
	virtual bool create(const EngineDesc& desc, const std::vector<IMiddlewarePtr>& middleware);

	// if this middleware should update or not
	virtual bool shouldUpdate();

	// the main polling loop for middleware
	virtual void update(Engine* engine);

};



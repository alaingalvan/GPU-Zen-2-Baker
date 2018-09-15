#include "IMiddleware.h"


IMiddleware::IMiddleware()
{

}

IMiddleware::~IMiddleware()
{

}

bool IMiddleware::create(const EngineDesc& desc, const std::vector<IMiddlewarePtr>& middleware)
{
	return false;
}


bool IMiddleware::shouldUpdate()
{
	return false;
}

void IMiddleware::update(Engine* engine)
{

}

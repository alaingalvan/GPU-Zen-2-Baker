#include "Actor.h"

Actor::Actor()
{
}

Actor::~Actor()
{
}

void Actor::addComponent(ComponentPtr component)
{
	mComponents.push_back(component);
}

void Actor::onSpawn()
{
}

void Actor::update(float deltaTime)
{
}

void Actor::onDestroy()
{
}

void Actor::setupState(const std::vector<IMiddlewarePtr>& middleware)
{
}

std::vector<ActorPtr>& Actor::getChildren()
{
	return mChildren;
}

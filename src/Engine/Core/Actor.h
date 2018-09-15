#pragma once

#include "Component.h"
#include "IMiddleware.h"

#include <vector>
#include <memory>

class Actor;
typedef std::shared_ptr<Actor> ActorPtr;

class Actor
{
public:
	Actor();
	virtual ~Actor();

	// Adds a component to an actor that can independently handle functionality.
	void addComponent(ComponentPtr component);

	// Lifetime hook that executes when the actor is spawned in a scene
	virtual void onSpawn();

	// A polling function that executes every frame
	virtual void update(float deltaTime);

	// Lifetime hook that executes when an actor is about to be removed from a scene
	virtual void onDestroy();

	// Allows an actor to subscribe to any of the state exposed by the Engine's middleware
	virtual void setupState(const std::vector<IMiddlewarePtr>& middleware);

	std::vector<ActorPtr>& getChildren();

protected:

	// Components managed by this actor
	std::vector<ComponentPtr> mComponents;

	// Children of this actor
	std::vector<ActorPtr> mChildren;
};
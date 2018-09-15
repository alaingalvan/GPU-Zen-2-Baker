#pragma once

#include <memory>


/**
 * A component is attached to an Actor, and is responsible for
 * encapsulating logic that otherwise would be handled by the Actor.
 * The idea is to make adding functionality as simple as adding a component.
 */
class Component
{
public:
	Component();

	virtual ~Component();
protected:

};

typedef std::shared_ptr<Component> ComponentPtr;

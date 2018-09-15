#pragma once

#include "Actor.h"

#include <vector>

class Scene
{
public:
	Scene();

	~Scene();

	void add(ActorPtr actor);

	size_t size();

	std::vector<ActorPtr>& getVector();

protected:
	std::vector<ActorPtr> mVector;
};

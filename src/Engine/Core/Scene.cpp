#include "Scene.h"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::add(ActorPtr actor)
{
	mVector.emplace_back(actor);
}

size_t Scene::size()
{
	return mVector.size();
}

std::vector<ActorPtr>& Scene::getVector()
{
	return mVector;
}

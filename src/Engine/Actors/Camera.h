#pragma once

#include "../Core/Actor.h"

#include "vectormath.hpp"

class Camera : public Actor
{
public:

	virtual void onSpawn() override;

	virtual void setupState(const std::vector<IMiddlewarePtr>& middleware) override;

	Matrix4 getViewProjection();

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	Matrix4 viewMatrix = Matrix4::identity();

	Matrix4 projectionMatrix = Matrix4::identity();
  
};

typedef std::shared_ptr<Camera> CameraPtr;

// 🏭 Factory function
ActorPtr camera();
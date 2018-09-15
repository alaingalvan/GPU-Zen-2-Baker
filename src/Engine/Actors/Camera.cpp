#include "Camera.h"
#include "../Middleware/Core.h"

void Camera::onSpawn()
{

}

void Camera::setupState(const std::vector<IMiddlewarePtr>& middleware)
{
	for (IMiddlewarePtr m : middleware)
	{
		middleware::CoreMiddlewarePtr core = std::dynamic_pointer_cast<pyro::middleware::CoreMiddleware>(m);
		if (core != nullptr)
		{
			core->renderer.setActiveCamera(CameraPtr(this));
			xgfx::RenderTarget* rt = core->renderer.getRenderTarget();
			xgfx::RenderTargetDesc desc = rt->getDesc();
			projectionMatrix = Matrix4::perspective(45.0f, static_cast<float>(desc.width) / static_cast<float>(desc.height), 0.01f, 1024.0f);

		}
	}
}

Matrix4 Camera::getViewProjection()
{
	return projectionMatrix * viewMatrix;
}

#include "Engine.h"


Engine::Engine()
{
	// 🎊 Add middleware
	mMiddleware =
	{
		coreMiddleware()
	};

	// 👨‍🎤 Setup actors
	mScene.add(baker());

}

Engine::~Engine()
{
}

void Engine::start(const EngineDesc& desc)
{
	// 🌟 Initialize middleware
	for (auto itr = mMiddleware.begin(); itr != mMiddleware.end();)
	{
		std::vector<IMiddlewarePtr> createdMiddleware(mMiddleware.begin(), itr);
		IMiddleware& m = **itr;
		if (!m.create(desc, createdMiddleware))
		{
			itr = mMiddleware.erase(itr);
			continue;
		}
		++itr;
	}

	// 🏎️ Run engine loop
	while (mRunning)
	{
		mRunning = false;

		for (IMiddlewarePtr& ware : mMiddleware)
		{
			bool shouldUpdate = ware->shouldUpdate();

			if (shouldUpdate)
			{
				mRunning |= shouldUpdate;
				ware->update(this);
			}
		}
	}
}

void Engine::stop()
{
	mRunning = false;
}

Scene& Engine::getScene()
{
	return mScene;
}

const std::vector<IMiddlewarePtr>& Engine::getMiddleware()
{
	return mMiddleware;
}
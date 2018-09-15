#include "CrossWindow/Main/Main.h"
#include "Engine/Engine.h"

void xmain(int argc, const char** argv)
{
	// 🔧 Configure the engine
	EngineDesc desc;
	desc.fullscreen = false;

	// 🏎️ Create an engine
	Engine engine;

	// 🏁 Start your engine
	engine.start(desc);
}
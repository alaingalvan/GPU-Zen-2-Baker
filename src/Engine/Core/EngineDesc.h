#pragma once

/**
 * How the engine should be configured when starting up,
 * this is passed to all middleware when they're created.
 */
struct EngineDesc
{
	bool fullscreen;
};
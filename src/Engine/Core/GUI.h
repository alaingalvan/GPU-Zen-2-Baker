#pragma once

#include "imgui.h"
#include "CrossWindow/Common/WindowDesc.h"
#include "CrossWindow/Common/Event.h"

/**
* GUI System
* Powered by IMGUI
* Adapted from https://github.com/ocornut/imgui/blob/master/examples/imgui_impl_opengl3.cpp
*/
class GUI
{

public:
	GUI();

	~GUI();

	bool create(xwin::WindowDesc wdesc);

	void update(float deltaTime);

	void updateEvent(xwin::Event e);

protected:

	void renderDrawData(ImDrawData* draw_data);

	bool createFontTexture();

	void destroyFontTexture();

	bool createDeviceObjects();

	void destroyDeviceObjects();

	char         mGLSLVersion[32] = "#version 150\n";
	unsigned     mFontTexture = 0;
	int          mShaderHandle = 0, mVertHandle = 0, mFragHandle = 0;
	int          mAttribLocationTex = 0, mAttribLocationProjMtx = 0;
	int          mAttribLocationPosition = 0, mAttribLocationUV = 0, mAttribLocationColor = 0;
	unsigned int mVboHandle = 0, mElementsHandle = 0;
};
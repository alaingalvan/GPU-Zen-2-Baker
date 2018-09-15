#include "GUI.h"
#include "../Engine.h"

#include <glad/glad.h>

GUI::GUI()
{
}

GUI::~GUI()
{
	destroyDeviceObjects();
}

bool GUI::create(xwin::WindowDesc wdesc)
{
	bool success = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	if (!mFontTexture)
	{
		if (!createDeviceObjects())
		{
			return false;
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built

	io.DisplaySize = ImVec2((float)wdesc.width, (float)wdesc.height);
	io.DisplayFramebufferScale = ImVec2(wdesc.width > 0 ? ((float)wdesc.width / wdesc.width) : 0, wdesc.width > 0 ? ((float)wdesc.width / wdesc.width) : 0);
	io.DeltaTime = (float)(1.0f / 60.0f);

	// Set up a new frame
	ImGui::NewFrame();

	return success;
}

void GUI::updateEvent(xwin::Event e)
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = ImVec2((float)0, (float)0);

	if (e.type == xwin::EventType::Resize)
	{
		io.DisplaySize.x = static_cast<float>(e.data.resize.width);
		io.DisplaySize.y = static_cast<float>(e.data.resize.height);
	}

	if (e.type == xwin::EventType::MouseInput)
	{
		xwin::MouseInputData& mid = e.data.mouseInput;

		if (mid.state == xwin::ButtonState::Pressed)
		{
			io.MouseDown[static_cast<size_t>(mid.button)] = 1;
		}
		else if (mid.state == xwin::ButtonState::Released)
		{
			io.MouseDown[static_cast<size_t>(mid.button)] = 0;
		}
	}

	if (e.type == xwin::EventType::MouseWheel)
	{
		xwin::MouseWheelData& mwd = e.data.mouseWheel;
		io.MouseWheel += static_cast<float>(mwd.delta);
	}

	if (e.type == xwin::EventType::Keyboard)
	{
		xwin::KeyboardData& kd = e.data.keyboard;

		if (kd.state == xwin::ButtonState::Pressed)
		{
			// map xwin events to win32 since IMGUI models after win32
			io.KeysDown[0] = 1;
		}
		else if (kd.state == xwin::ButtonState::Released)
		{
			io.KeysDown[0] = 0;
		}
	}
}

void GUI::update(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();

	io.DeltaTime = deltaTime;

	ImGui::EndFrame();

	ImGui::Render();

	renderDrawData(ImGui::GetDrawData());

	// Start a new UI frame
	ImGui::NewFrame();
}

void GUI::renderDrawData(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float ortho_projection[4][4] =
	{
		{ 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f },
	};
	glUseProgram(mShaderHandle);
	glUniform1i(mAttribLocationTex, 0);
	glUniformMatrix4fv(mAttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	if (glBindSampler) glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.

											// Recreate the VAO every time 
											// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
	GLuint vao_handle = 0;
	glGenVertexArrays(1, &vao_handle);
	glBindVertexArray(vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
	glEnableVertexAttribArray(mAttribLocationPosition);
	glEnableVertexAttribArray(mAttribLocationUV);
	glEnableVertexAttribArray(mAttribLocationColor);
	glVertexAttribPointer(mAttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(mAttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(mAttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	ImVec2 pos = draw_data->DisplayPos;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				// User callback (registered via ImDrawList::AddCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					// Apply scissor/clipping rectangle
					glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

					// Bind texture, Draw
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}
	glDeleteVertexArrays(1, &vao_handle);

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	if (glBindSampler) glBindSampler(0, last_sampler);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool GUI::createFontTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;

	/**
	* Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely
	* to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than
	* just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	*/
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);


	// Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &mFontTexture);
	glBindTexture(GL_TEXTURE_2D, mFontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)mFontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

void GUI::destroyFontTexture()
{
	if (mFontTexture)
	{
		ImGuiIO& io = ImGui::GetIO();
		glDeleteTextures(1, &mFontTexture);
		io.Fonts->TexID = 0;
		mFontTexture = 0;
	}
}

bool GUI::createDeviceObjects()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	// Create shaders
	const GLchar *vertex_shader =
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	const GLchar* vertex_shader_with_version[2] = { mGLSLVersion, vertex_shader };
	const GLchar* fragment_shader_with_version[2] = { mGLSLVersion, fragment_shader };

	mShaderHandle = glCreateProgram();
	mVertHandle = glCreateShader(GL_VERTEX_SHADER);
	mFragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mVertHandle, 2, vertex_shader_with_version, NULL);
	glShaderSource(mFragHandle, 2, fragment_shader_with_version, NULL);
	glCompileShader(mVertHandle);
	glCompileShader(mFragHandle);
	glAttachShader(mShaderHandle, mVertHandle);
	glAttachShader(mShaderHandle, mFragHandle);
	glLinkProgram(mShaderHandle);

	mAttribLocationTex = glGetUniformLocation(mShaderHandle, "Texture");
	mAttribLocationProjMtx = glGetUniformLocation(mShaderHandle, "ProjMtx");
	mAttribLocationPosition = glGetAttribLocation(mShaderHandle, "Position");
	mAttribLocationUV = glGetAttribLocation(mShaderHandle, "UV");
	mAttribLocationColor = glGetAttribLocation(mShaderHandle, "Color");

	glGenBuffers(1, &mVboHandle);
	glGenBuffers(1, &mElementsHandle);

	createFontTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}

void GUI::destroyDeviceObjects()
{
	if (mVboHandle) glDeleteBuffers(1, &mVboHandle);
	if (mElementsHandle) glDeleteBuffers(1, &mElementsHandle);
	mVboHandle = mElementsHandle = 0;

	if (mShaderHandle && mVertHandle) glDetachShader(mShaderHandle, mVertHandle);
	if (mVertHandle) glDeleteShader(mVertHandle);
	mVertHandle = 0;

	if (mShaderHandle && mFragHandle) glDetachShader(mShaderHandle, mFragHandle);
	if (mFragHandle) glDeleteShader(mFragHandle);
	mFragHandle = 0;

	if (mShaderHandle) glDeleteProgram(mShaderHandle);
	mShaderHandle = 0;

	destroyFontTexture();
}

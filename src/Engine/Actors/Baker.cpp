#include "Baker.h"

#include <glad/glad.h>
#include "imgui.h"

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

// Create an instance of the Importer class
Assimp::Importer importer;
const aiScene* scene;
#include <fstream>
#include <iostream>

void logInfo(std::string logString)
{
	// Will add message to File with "info" Tag
	Assimp::DefaultLogger::get()->info(logString.c_str());
}


bool Import3DFromFile(const std::string& pFile)
{
	// Check if file exists
	std::ifstream fin(pFile.c_str());
	if (!fin.fail())
	{
		fin.close();
	}
	else
	{
		MessageBox(NULL, ("Couldn't open file: " + pFile).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		logInfo(importer.GetErrorString());
		return false;
	}

	scene = importer.ReadFile(pFile, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if (!scene)
	{
		logInfo(importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	logInfo("Import of scene " + pFile + " succeeded.");

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}


Baker::Baker()
{
	mTime = 0.0f;

	mVertexArray = 0;
	mProgram = 0;
	mVertexShader = 0;
	mFragmentShader = 0;
	mVertexBuffer = 0;
	mIndexBuffer = 0;
	mPositionAttrib = 0;

	// Change this line to normal if you not want to analyse the import process
	//Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

	// Create a logger instance for Console Output
	Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

	// Create a logger instance for File Output (found in project folder or near .exe)
	//Assimp::DefaultLogger::create("assimp_log.txt", severity, aiDefaultLogStream_FILE);

	// Now I am ready for logging my stuff
	Assimp::DefaultLogger::get()->info("this is my info-call");

	std::string path = getAppDirectory() + "assets/models/brick-cube-low.fbx";
	Import3DFromFile(path);
}

void Baker::recursive_render(const struct aiScene *sc, const struct aiNode* nd)
{
	/*
	unsigned int n = 0;

	for (; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		if (mVertexBuffer == 0)
		{
			if (mesh->HasPositions())
			{
				glGenBuffers(1, &mVertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(aiVector3D) * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);

				mPositionAttrib = glGetAttribLocation(mProgram, "inPos");
				glEnableVertexAttribArray(mPositionAttrib);
				glVertexAttribPointer(mPositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), 0);
			}
		}

		if (mIndexBuffer == 0)
		{
			if (mesh->HasFaces())
			{
				for (size_t i = 0; i < mesh->mNumFaces; ++i
				{
					for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
					{
						mIndexBufferData.emplace_back(mesh->mFaces[i].mIndices[j]);
					}
				}
				glGenBuffers(1, &mIndexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * mIndexBufferData.size(), mIndexBufferData.data(), GL_STATIC_DRAW);
			}
		}

		if (mesh->HasFaces())
		{
			glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
			glDrawElements(GL_TRIANGLES, (unsigned)mIndexBufferData.size(), GL_UNSIGNED_INT, 0);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		recursive_render(sc, nd->mChildren[n]);
	}
	*/
}

void Baker::onSpawn()
{
	mCamera = camera();
	addChild(mCamera);
}

void Baker::update(float deltaTime)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	mTime += deltaTime;
	ImGui::Text("Elapsed Time: %.3f", mTime);

	// Drawing mesh

	if (mVertexArray == 0)
	{
		glGenVertexArrays(1, &mVertexArray);
	}
	glBindVertexArray(mVertexArray);

	auto checkShaderCompilation = [&](GLuint shader)
	{
#if defined(_DEBUG)
		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(shader);

			std::cout << errorLog.data();

			return false;
		}
#endif
		return true;
	};

	if (mVertexShader == 0)
	{
		std::string path = getAppDirectory() + "assets/shaders/MaterialVertexColor.vert";

		std::vector<char> vertShaderCode = readFile(path);
		GLchar* vertStr = vertShaderCode.data();
		GLint vertLen = static_cast<GLint>(vertShaderCode.size());

		mVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(mVertexShader, 1, &vertStr, &vertLen);
		glCompileShader(mVertexShader);
		if (!checkShaderCompilation(mVertexShader)) return;
	}

	if (mFragmentShader == 0)
	{
		std::string path = getAppDirectory() + "assets/shaders/MaterialVertexColor.frag";

		std::vector<char> fragShaderCode = readFile(path);
		GLchar* fragStr = fragShaderCode.data();
		GLint fragLen = static_cast<GLint>(fragShaderCode.size());

		mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(mFragmentShader, 1, &fragStr, &fragLen);
		glCompileShader(mFragmentShader);
		if (!checkShaderCompilation(mFragmentShader)) return;
	}

	if (mProgram == 0)
	{
		mProgram = glCreateProgram();
		glAttachShader(mProgram, mVertexShader);
		glAttachShader(mProgram, mFragmentShader);
		glLinkProgram(mProgram);

		GLint result = 0;
		glGetProgramiv(mProgram, GL_LINK_STATUS, &result);
#if defined(_DEBUG)
		if (result != GL_TRUE) {
			std::cout << "Program failed to link.";
			return;
		}
#endif
	}
	glUseProgram(mProgram);
	glEnableVertexAttribArray(mVertexArray);

	recursive_render(scene, scene->mRootNode);

	glEnableVertexAttribArray(0);
	glUseProgram(0);
}

ActorPtr baker()
{
	return ActorPtr(new Baker());
}


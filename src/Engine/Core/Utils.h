#pragma once

#include <vector>
#include <fstream>

#if defined(XWIN_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

// Common Utils

inline std::string getAppDirectory()
{
	std::string out;
	char pBuf[2048];
#ifdef XWIN_WIN32
	_getcwd(pBuf, 2048);
	out = pBuf;
	out += "\\";
#else
	getcwd(pBuf, 2048);
	out = pBuf;
	out += "/";
#endif
	return out;
}

// Read a target file
inline std::vector<char> readFile(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	bool exists = (bool)file;

	if (!exists || !file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;

}

// clamp value between two other values
template <typename T>
inline T clamp(const T& value, const T& low, const T& high)
{
	return value < low ? low : (value > high ? high : value);
}

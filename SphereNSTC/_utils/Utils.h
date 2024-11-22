#pragma once

#include "sdl3/SDL.h"
#include "Math.h"
#include "Time.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#define SDL_CHECK(x)												        																\
	do{				    											        																			\
		if (!(bool)x) {																																	\
			std::cerr << "Detected SDL error: " << SDL_GetError() << std::endl;						\
		}															        																					\
	}while(0)

typedef glm::vec4 Color;

typedef int Index;

struct Vertex {
	glm::vec3 position;
	float u;
	glm::vec3 normal;
	float v;
	Color color;
};

static inline std::string readFileText(const char* path) {
	std::ifstream file(path, std::ios::ate | std::ios::in);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to read file bytes.");
	}
	
	size_t fileSize = (size_t)file.tellg();
	std::string text;
	
	file.seekg(0);
	file.read(text.data(), fileSize);
	file.close();

	return text;
}

static inline std::vector<uint32_t> readFileBytes(const char* path) {
	std::ifstream file(path, std::ios::ate | std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to read file bytes.");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> fileBytes(fileSize / sizeof(uint32_t));

	file.seekg(0);
	file.read((char*)fileBytes.data(), fileSize);
	file.close();

	return fileBytes;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "(" << v.x << "," << v.y << "," << v.z << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
	os << "(" << v.x << "," << v.y << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
	for (int i = 0; i < m.length(); i++) {
		for (int j = 0; j < m[i].length(); j++) {
			os << m[i][j] << " ";
		}
		os << "\n";
	}
	return os;
}
#include "assets_manager.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>

namespace assets {

AssetsManager::AssetsManager() {
	char *base = SDL_GetBasePath();
	m_base_path = base;
	SDL_free(base);
}

rendering::Shader& AssetsManager::get_shader(const std::string &name) {
	auto it = m_shaders.find(name);

	if (it != m_shaders.end()) {
		return *it->second;
	}

	std::string path = m_base_path + "shaders/" + name + ".glsl";
	std::ifstream file(path);
	std::string src = (std::stringstream() << file.rdbuf()).str();
	// std::cout << src << std::endl;
	auto r = m_shaders.insert({name, std::make_unique<rendering::Shader>(src)});
	return *r.first->second;
}



}
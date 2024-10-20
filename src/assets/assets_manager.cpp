#include "assets_manager.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL2/SDL.h>

#include "render/obj_reader.hpp"

namespace assets {

AssetsManager::AssetsManager() {
	char *base = SDL_GetBasePath();
	m_base_path = base;
	SDL_free(base);
}

rendering::Shader& AssetsManager::get_shader(const std::string& name) {
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

rendering::PosNormMesh& AssetsManager::get_model(const std::string& name) {
	auto it = m_meshes.find(name);
	if (it != m_meshes.end()) {
		return *it->second;
	}

	std::string path = m_base_path + "models/" + name + ".obj";
	std::ifstream file(path);

	auto r = m_meshes.insert({name, std::make_unique<rendering::PosNormMesh>()});
	rendering::PosNormMesh& mesh = *r.first->second;
	rendering::read_obj(file, mesh);

	return mesh;
}

rendering::TextAtlas& AssetsManager::get_atlas(const std::string& font) {
	auto it = m_texts.find(font);
	if (it != m_texts.end()) {
		return *it->second;
	}

	std::string path = m_base_path + "fonts/" + font + ".ttf";
	auto fontface = m_text.load_font(path);
	auto r = m_texts.insert({font, std::make_unique<rendering::TextAtlas>()});
	rendering::TextAtlas& atlas = *r.first->second;
	atlas.load(fontface);

	return atlas;
}


}
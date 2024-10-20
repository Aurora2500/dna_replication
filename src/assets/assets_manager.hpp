#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "text.hpp"

#include "render/shader.hpp"
#include "render/mesh.hpp"
#include "render/text.hpp"

namespace assets {

class AssetsManager {
private:
	std::string m_base_path;
	TextLibrary m_text;
	std::unordered_map<std::string, std::unique_ptr<rendering::Shader>> m_shaders;
	std::unordered_map<std::string, std::unique_ptr<rendering::PosNormMesh>> m_meshes;
	std::unordered_map<std::string, std::unique_ptr<rendering::TextAtlas>> m_texts;

public:
	AssetsManager();
	rendering::Shader& get_shader(const std::string& name);
	rendering::PosNormMesh& get_model(const std::string& name);
	rendering::TextAtlas& get_atlas(const std::string& font);
};

}
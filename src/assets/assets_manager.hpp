#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "render/shader.hpp"
#include "render/mesh.hpp"

namespace assets {

class AssetsManager {
private:
	std::string m_base_path;
	std::unordered_map<std::string, std::unique_ptr<rendering::Shader>> m_shaders;
	std::unordered_map<std::string, std::unique_ptr<rendering::PosNormMesh>> m_meshes;

public:
	AssetsManager();
	rendering::Shader& get_shader(const std::string& name);
	rendering::PosNormMesh& get_model(const std::string& name);
};

}
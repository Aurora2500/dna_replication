#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "render/shader.hpp"

namespace assets {

class AssetsManager {
private:
	std::string m_base_path;
	std::unordered_map<std::string, std::unique_ptr<rendering::Shader>> m_shaders;

public:
	AssetsManager();
	rendering::Shader& get_shader(const std::string& name);
};

}
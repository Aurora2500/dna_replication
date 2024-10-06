#pragma once

#include "assets/assets_manager.hpp"

#include "model/strand.hpp"

#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/shader.hpp"
#include "render/ssbo.hpp"

class strand_view {
private:
	Strand m_strand;

	rendering::SSBO m_nucleobase_ssbo;
	rendering::SSBO m_control_point_ssbo;
	std::vector<glm::vec4> m_control_points;

	rendering::SimpleMesh m_backbone_mesh;
	rendering::PosNormMesh m_nucleobase_mesh;

	void upload_nucleobases();
public:
	strand_view(Strand&& strand, const std::vector<glm::vec4>& control_points);


	void draw(glm::mat4& vp, assets::AssetsManager& assets);

};
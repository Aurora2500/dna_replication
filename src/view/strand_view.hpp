#pragma once

#include "assets/assets_manager.hpp"

#include "model/strand.hpp"
#include "model/proteins.hpp"

#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/shader.hpp"
#include "render/ssbo.hpp"

#include "util/iter_map.hpp"

class strand_view {
private:
	Strand m_strand;

	iter_map<gap, Helicase&> m_reverse_helicase_map;
	iter_map<interval, Polymerase&> m_reverse_polymerase_map;
	std::vector<Helicase> m_helicases;
	std::vector<Polymerase> m_polymerases;

	rendering::SSBO m_nucleobase_ssbo;
	rendering::SSBO m_control_point_ssbo;
	std::vector<glm::vec4> m_control_points;

	rendering::SimpleMesh m_backbone_mesh;
	rendering::PosNormMesh m_nucleobase_mesh;

	void upload_nucleobases();
public:
	strand_view(Strand&& strand, const std::vector<glm::vec4>& control_points);

	void update(float dt);
	void draw(glm::mat4& vp, assets::AssetsManager& assets);

};
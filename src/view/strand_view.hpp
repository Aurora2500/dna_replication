#pragma once

#include "assets/assets_manager.hpp"

#include "model/strand.hpp"
#include "model/proteins.hpp"

#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/shader.hpp"
#include "render/ssbo.hpp"

template<typename T>
struct IteratorHash {
	std::size_t operator()(const typename std::list<T>::iterator& it) const noexcept;
};

class strand_view {
private:
	Strand m_strand;

	std::unordered_map<std::list<gap>::iterator, Helicase&, IteratorHash<gap>> m_reverse_helicase_map;
	std::unordered_map<std::list<interval>::iterator, Polymerase&, IteratorHash<interval>> m_reverse_polymerase_map;
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
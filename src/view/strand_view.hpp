#pragma once

#include <array>

#include "assets/assets_manager.hpp"

#include "debug/line.hpp"

#include "math/bspline.hpp"

#include "model/strand.hpp"
#include "model/proteins.hpp"

#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/shader.hpp"
#include "render/ssbo.hpp"

#include "util/iter_map.hpp"

#include "windowing/events.hpp"

#include "camera_controller.hpp"
#include "gap_spline_bridge.hpp"

class strand_view {
private:
	Strand m_strand;

	iter_map<gap, Helicase&> m_reverse_helicase_map;
	iter_map<interval, Polymerase&> m_reverse_polymerase_map;
	std::vector<Helicase> m_helicases;
	std::vector<Polymerase> m_polymerases;
	std::vector<Ligase> m_ligases;

	unsigned int m_num_ctrl_points;
	bspline_network m_spline;
	gap_spline_bridge m_bridge;
	std::array<std::vector<glm::vec4>, 2> m_ctrl_point_cache;
	std::array<rendering::SSBO, 2> m_control_point_ssbos;

	CameraController m_cam;

	rendering::SSBO m_nucleobase_ssbo;

	rendering::SimpleMesh m_backbone_mesh;
	rendering::PosNormMesh m_nucleobase_mesh;

	debug::line m_dbg_line;

	bool m_paused;

	void upload_nucleobases();

	void update_helicase_expansion(float dt);
	void update_bspline();

	void draw_base_dna(glm::mat4& vp, assets::AssetsManager& assets);
	void draw_helicase(glm::mat4& vp, assets::AssetsManager& assets);

	void draw_debug(glm::mat4& vp, assets::AssetsManager& assets);
public:
	strand_view(Strand&& strand, float aspect);

	void handle_events(EventManager& events);
	void update(float dt);
	void draw(assets::AssetsManager& assets);

};
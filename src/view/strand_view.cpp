#include "strand_view.hpp"

#include <cmath>
#include <ranges>
#include <iostream>
#include <random>

#include "domain_mesh.hpp"

constexpr float HELICASE_SPEED = 1;
constexpr float POLYMERASE_SPEED = 1;
constexpr float LIGASE_TIME = 4;

void strand_view::upload_nucleobases() {
	std::vector<unsigned int> packed_nucleobase_buffer;

	int offset = 0;
	unsigned int packet = 0;
	for (auto nucleobase : m_strand.nucleobases()) {
		packet |= static_cast<char>(nucleobase) << (offset * 2);
		offset++;
		if (offset == 16) {
			offset = 0;
			packed_nucleobase_buffer.push_back(packet);
			packet = 0;
		}
	}
	if (offset != 0) {
		packed_nucleobase_buffer.push_back(packet);
	}

	m_nucleobase_ssbo.set(packed_nucleobase_buffer);
}

strand_view::strand_view(Strand&& strand, float aspect)
	: m_strand(strand)
	, m_backbone_mesh(create_backbone_mesh(50, 16, 0.1))
	, m_nucleobase_mesh(create_nucleobase_mesh(glm::vec3(0.08, 0.45, 0.1)))
	, m_reverse_helicase_map()
	, m_reverse_polymerase_map()
	// 4 nucleobases per segment, num of ctrl points is 3 higher than num of segments, ceil(x/y) = (x+y-1) / y
	, m_num_ctrl_points( (strand.nucleobases().size() + 3) / 4 + 3 )
	, m_spline(m_num_ctrl_points)
	, m_bridge(m_spline)
	, m_dbg_line({glm::vec4(0.), glm::vec4(0.,0.,1.,0.)})
	, m_cam(aspect)
	, m_paused(false)
	, m_set_speed(0)
{
	upload_nucleobases();
	for (int i = 0; i < 2; i++) {
		m_ctrl_point_cache[i].reserve(m_num_ctrl_points);
		m_control_point_ssbos[i].allocate<glm::vec4>(m_num_ctrl_points);
	}

	auto& helicase = m_helicases.emplace_back();
	helicase.attach(true, m_strand.create_gap(0.));
	auto split_it = m_spline.create_gap(0);
	m_bridge.associate(*helicase.attachment(), split_it);
}

void strand_view::update_helicase_expansion(float dt) {
	// expand gaps trough helicases
	for (auto& helicase: m_helicases) {
		if (helicase.attachment()) {
			m_bridge.helicase_expansion(helicase, HELICASE_SPEED * dt);
		}
	}
	// merge gaps
	{
		auto& gaps = m_strand.gaps();
		auto it = gaps.begin();
		auto gap_end = gaps.end();
		while (it != gap_end) {
			auto& curr = *it;
			it++;
			if (it == gap_end) break;
			auto&next = *it;
			if (!curr.gap_size.overlaps(next.gap_size)) continue;
			curr.gap_size.higher = next.gap_size.higher;
			for (int i = 0; i < 2; i++) {
				curr.okazaki_fragments[i].splice(curr.okazaki_fragments[i].end(), next.okazaki_fragments[i]);
			}
			gaps.erase(it--);
		}
	}
}

void strand_view::update_polymerase_expansion(float dt) {
	for (auto& polymerase : m_polymerases) {
		if (polymerase.attachment()) {
			polymerase.expand(POLYMERASE_SPEED * dt);
		}
	}
}

void strand_view::update_ligase_adding(float dt) {
	for (auto& ligase : m_ligases) {
		if (ligase.attachment()) {
			ligase.tick(dt);
		}
	}
}

constexpr float MIN_HELICASE_GAP = 10;

void strand_view::update_helicase_binding() {
	std::random_device rng_dev;
	std::mt19937 rng(rng_dev());
	for (auto& helicase : m_helicases) {
		if (helicase.attachment()) continue;
		// find attachment spot
		auto& gaps = m_strand.gaps();
		float start;
		float end;

		auto it = gaps.begin();
		if (it == gaps.end()) {
			start = 0;
			end = m_num_ctrl_points - 3;
		} else {
			bool found = false;
			while (true) {
				auto next = std::next(it);
				if (next == gaps.end()) break;
				auto gap_size = next->gap_size.lower - it->gap_size.higher;
				if (gap_size >= MIN_HELICASE_GAP) {
					start = it->gap_size.higher;
					end = it->gap_size.lower;
					break;
				}
				it = next;
			}
			if (!found) {
				break;
			}
		}

		std::uniform_real_distribution uniform(start, end - MIN_HELICASE_GAP);
		float split_point = uniform(rng);
	}
}

void strand_view::update_polymerase_binding() {

}

void strand_view::update_ligase_binding() {

}

void strand_view::update_bspline() {
	for (int i = 0; i < 2; i++) {
		auto spline_points = m_spline.iter(i);
		m_ctrl_point_cache[i].assign(spline_points.begin(), spline_points.end());
		m_control_point_ssbos[i].update(m_ctrl_point_cache[i]);
	}
}

void strand_view::draw_base_dna(glm::mat4& vp, assets::AssetsManager& assets) {
	// backbone
	auto& backbone_shader = assets.get_shader("backbone");
	backbone_shader.use();
	backbone_shader.set_uniform("vp", vp);
	backbone_shader.set_uniform("offset_pos", glm::vec2(0., -0.5));
	backbone_shader.set_uniform("range", glm::vec2(0., 1.));
	backbone_shader.set_uniform("id_offset", 0);

	backbone_shader.set_uniform("complement", 0);
	for (int i = 0; i < 2; i++) {
		m_control_point_ssbos[i].bind_shader(0);
		backbone_shader.set_uniform("complement", i);
		m_backbone_mesh.draw_instanced(m_num_ctrl_points - 3);
	}

	// nucleobase
	m_nucleobase_ssbo.bind_shader(1);
	auto& nucleobase_shader = assets.get_shader("nucleobase");
	nucleobase_shader.use();
	nucleobase_shader.set_uniform("vp", vp);
	nucleobase_shader.set_uniform("param_offset", glm::vec2(0.125, 0.25));
	nucleobase_shader.set_uniform("offset_pos", glm::vec2(0., -0.5));
	nucleobase_shader.set_uniform("id_offset", 0);

	for (int i = 0; i < 2; i++) {
		m_control_point_ssbos[i].bind_shader(0);
		nucleobase_shader.set_uniform("complement", i);
		m_nucleobase_mesh.draw_instanced(m_strand.nucleobases().size());
	}
}

void strand_view::draw_complement_dna(glm::mat4& vp, assets::AssetsManager& assets) {
	//backbones
	auto& backbone_shader = assets.get_shader("backbone");
	backbone_shader.use();
	backbone_shader.set_uniform("vp", vp);
	backbone_shader.set_uniform("offset_pos", glm::vec2(0, -0.5));
	for (int i = 0; i < 2; i++) {
		m_control_point_ssbos[i].bind_shader(0);
		backbone_shader.set_uniform("complement", 1-i);
		for (auto&gap : m_strand.gaps()) {
			for (auto&fragment: gap.okazaki_fragments[i]) {
				float start = fragment.lower;
				int start_floor = std::floor(start);
				int start_ceil = std::ceil(start);
				float end = fragment.higher;
				int end_floor = std::floor(end);

				backbone_shader.set_uniform("id_offset", start_floor);
				backbone_shader.set_uniform("range", glm::vec2(start - start_floor, 1));
				m_backbone_mesh.draw();

				backbone_shader.set_uniform("id_offset", start_ceil);
				backbone_shader.set_uniform("range", glm::vec2(0., 1.));
				m_backbone_mesh.draw_instanced(end_floor - start_ceil);

				backbone_shader.set_uniform("id_offset", end_floor);
				backbone_shader.set_uniform("range", glm::vec2(0., end - end_floor));
				m_backbone_mesh.draw();
			}
		}
	}
}

void strand_view::draw_helicase(glm::mat4& vp, assets::AssetsManager& assets) {
	auto& obj_shader = assets.get_shader("object_attached");
	auto& helicase_mesh = assets.get_model("helicase");
	obj_shader.use();
	obj_shader.set_uniform("vp", vp);
	obj_shader.set_uniform("surface_color", glm::vec3(0.3, 0.7, 0.3));
	m_control_point_ssbos[0].bind_shader(0);
	for (auto &helicase : m_helicases) {
		auto attached_gap = helicase.attachment();
		if (!attached_gap) continue;
		auto gap = **attached_gap;
		auto ascending = helicase.ascending();
		auto param = (ascending ? gap.gap_size.higher : gap.gap_size.lower);
		obj_shader.set_uniform("param", param);
		obj_shader.set_uniform("reverse", ascending ? 0 : 1);
		obj_shader.set_uniform("flip", 0);
		helicase_mesh.draw();
	}
}

void strand_view::draw_polymerase(glm::mat4& vp, assets::AssetsManager& assets) {
	auto& obj_shader = assets.get_shader("object_attached");
	auto& polymerase_mesh = assets.get_model("polymerase");
	obj_shader.use();
	obj_shader.set_uniform("vp", vp);
	obj_shader.set_uniform("surface_color", glm::vec3(0.7, 0.3, 0.3));
	for (auto& polymerase : m_polymerases) {
		auto attached_frag = polymerase.attachment();
		if (!attached_frag) continue;
		auto fragment = **attached_frag;
		auto dir = polymerase.direction();
		auto param = (dir == Direction::Three ? fragment.lower : fragment.higher);
		obj_shader.set_uniform("param", param);
		obj_shader.set_uniform("reverse", dir == Direction::Three ? 1 : 0);
		obj_shader.set_uniform("flip", dir == Direction::Three ? 1 : 0);
		m_control_point_ssbos[dir == Direction::Three ? 1 : 0].bind_shader(0);
		polymerase_mesh.draw();
	}
}

void strand_view::draw_ligase(glm::mat4& vp, assets::AssetsManager& assets) {
	auto& obj_shader = assets.get_shader("object_attached");
	auto& ligase_mesh = assets.get_model("ligase");
	obj_shader.use();
	obj_shader.set_uniform("vp", vp);
	obj_shader.set_uniform("surface_color", glm::vec3(0.3, 0.3, 0.7));
	for (auto& ligase : m_ligases) {
		auto attached_frag = ligase.attachment();
		if (!attached_frag) continue;
		auto fragment = **attached_frag;
		auto dir = ligase.direction();
		auto param = (dir == Direction::Three ? fragment.lower : fragment.higher);
		obj_shader.set_uniform("param", param);
		obj_shader.set_uniform("reverse", dir == Direction::Three ? 1 : 0);
		obj_shader.set_uniform("flip", dir == Direction::Three ? 1 : 0);
		m_control_point_ssbos[dir == Direction::Three ? 1 : 0].bind_shader(0);
		ligase_mesh.draw();
	}

}

void strand_view::draw_debug(glm::mat4& vp, assets::AssetsManager& assets) {
	auto& line_shader = assets.get_shader("line");
	line_shader.use();
	line_shader.set_uniform("vp", vp);
	line_shader.set_uniform("line_col", glm::vec3(1., 0., 0.));
	for (int i = 0; i < 2; i++) {
		m_control_point_ssbos[i].bind_shader(0);
		m_dbg_line.draw(m_num_ctrl_points - 1);
	}
}

void strand_view::handle_events(EventManager& events) {
	m_cam.handle_events(events);
	if (events.test_flag(EventKind::Keyboard))
		for(auto& e: events.events()) {
			if (e.kind != EventKind::Keyboard || !e.keyboard.down) continue;
			switch (e.keyboard.key)
			{
			case SDLK_SPACE:
				m_paused = !m_paused;
				break;
			case SDLK_1:
				m_set_speed = 0.2;
				break;
			case SDLK_2:
				m_set_speed = 0.5;
				break;
			case SDLK_3:
				m_set_speed = 1;
				break;
			case SDLK_4:
				m_set_speed = 2;
				break;
			case SDLK_5:
				m_set_speed = 4;
				break;
			case SDLK_6:
				m_set_speed = 16;
				break;
			}
		}
}

void strand_view::update(float dt) {
	if (m_paused) return;
	float warp_dt = m_set_speed * dt;
	update_helicase_expansion(warp_dt);
	m_spline.update(warp_dt);

	update_bspline();
}

void strand_view::draw(assets::AssetsManager& assets) {
	auto vp = m_cam.cam.get_view_matrix();
	draw_base_dna(vp, assets);
	draw_complement_dna(vp, assets);
	draw_helicase(vp, assets);
	draw_polymerase(vp, assets);
	draw_ligase(vp, assets);
	draw_debug(vp, assets);

}
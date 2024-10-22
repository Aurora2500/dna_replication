#include "strand_view.hpp"

#include <ranges>
#include <iostream>

#include "domain_mesh.hpp"

constexpr float HELICASE_SPEED = 1;
constexpr float POLYMERASE_SPEED = 1;

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
			m_reverse_helicase_map.find(it);
			gaps.erase(it--);
		}
	}
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

	m_control_point_ssbos[0].bind_shader(0);
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

	for (int i = 0; i < 2; i++) {
		m_control_point_ssbos[i].bind_shader(0);
		nucleobase_shader.set_uniform("complement", i);
		m_nucleobase_mesh.draw_instanced(m_strand.nucleobases().size());
	}
}

void strand_view::draw_helicase(glm::mat4& vp, assets::AssetsManager& assets) {
	auto& obj_shader = assets.get_shader("object_attached");
	auto& helicase_mesh = assets.get_model("helicase");
	obj_shader.use();
	obj_shader.set_uniform("vp", vp);
	obj_shader.set_uniform("surface_color", glm::vec3(0.3, 0.7, 0.3));
	for (auto &helicase : m_helicases) {
		auto attached_gap = helicase.attachment();
		if (!attached_gap) continue;
		auto gap = **attached_gap;
		auto ascending = helicase.ascending();
		auto param = (ascending ? gap.gap_size.higher : gap.gap_size.lower);
		obj_shader.set_uniform("param", param);
		helicase_mesh.draw();
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
			if (e.kind == EventKind::Keyboard && e.keyboard.key == SDLK_SPACE && e.keyboard.down) {
				m_paused = !m_paused;
			}
		}
}

void strand_view::update(float dt) {
	if (m_paused) return;
	update_helicase_expansion(dt);
	m_spline.update(dt);

	update_bspline();
}

void strand_view::draw(assets::AssetsManager& assets) {
	auto vp = m_cam.cam.get_view_matrix();
	draw_base_dna(vp, assets);
	draw_helicase(vp, assets);
	draw_debug(vp, assets);

}
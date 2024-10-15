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

	m_nucleobase_ssbo.set_data(
		packed_nucleobase_buffer.data(),
		packed_nucleobase_buffer.size() * sizeof(unsigned int)
	);
}

strand_view::strand_view(Strand&& strand, const std::vector<glm::vec4>& control_points)
	: m_strand(strand)
	, m_control_points(control_points)
	, m_backbone_mesh(create_backbone_mesh(50, 16, 0.1))
	, m_nucleobase_mesh(create_nucleobase_mesh(glm::vec3(0.08, 0.45, 0.1)))
	, m_reverse_helicase_map()
	, m_reverse_polymerase_map()
{
	upload_nucleobases();
	m_control_point_ssbo.set_data(m_control_points.data(), m_control_points.size() * sizeof(glm::vec4));

	auto& helicase = m_helicases.emplace_back();
	helicase.attach(true, m_strand.create_gap(0.));
}

void strand_view::update(float dt) {
	// expand gaps trough helicases
	for (auto& helicase: m_helicases) {
		helicase.expand(HELICASE_SPEED * dt);
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

void strand_view::draw(glm::mat4& vp, assets::AssetsManager& assets) {

	// render backbone
	auto& backbone_shader = assets.get_shader("backbone");
	m_control_point_ssbo.bind_shader(0);

	backbone_shader.use();
	backbone_shader.set_uniform("vp", vp);
	backbone_shader.set_uniform("offset_pos", glm::vec2(0., -0.5));

	backbone_shader.set_uniform("complement", 0);
	m_backbone_mesh.draw_instanced(m_control_points.size()-3);
	backbone_shader.set_uniform("complement", 1);
	m_backbone_mesh.draw_instanced(m_control_points.size()-3);

	//render nucleobases
	m_nucleobase_ssbo.bind_shader(1);
	auto& nucleobase_shader = assets.get_shader("nucleobase");
	nucleobase_shader.use();
	nucleobase_shader.set_uniform("vp", vp);
	nucleobase_shader.set_uniform("param_offset", glm::vec2(0.125, 0.25));
	nucleobase_shader.set_uniform("offset_pos", glm::vec2(0., -0.5));

	nucleobase_shader.set_uniform("complement", 0);
	m_nucleobase_mesh.draw_instanced(m_strand.nucleobases().size());
	nucleobase_shader.set_uniform("complement", 1);
	m_nucleobase_mesh.draw_instanced(m_strand.nucleobases().size());

	// rendering helicases
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
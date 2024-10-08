#include "strand_view.hpp"

#include "domain_mesh.hpp"

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
{
	upload_nucleobases();
	m_control_point_ssbo.set_data(m_control_points.data(), m_control_points.size() * sizeof(glm::vec4));
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
}
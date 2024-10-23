#include "event_log.hpp"

constexpr float EVENT_TIME = 9;

event::event(rendering::Mesh2D&& mesh, float time) 
	: text_mesh(std::move(mesh))
	,	time_left(time)
{ }

event_log::event_log(rendering::TextAtlas& atlas)
	: m_atlas(atlas)
{}

void event_log::update(float dt) {
	for (auto it = m_events.begin(); it != m_events.end();) {
		auto& event = *it;
		event.time_left -= dt;
		auto del_it = it;
		it++;
		if (event.time_left <= 0) {
			m_events.erase(del_it);
		}
	}
}

void event_log::add_event(const std::string& event_message) {
	auto text_mesh = m_atlas.create_text(event_message, 1);
	m_events.push_front(event(std::move(text_mesh), EVENT_TIME));
}
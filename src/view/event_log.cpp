#include "event_log.hpp"

constexpr float EVENT_TIME = 9;

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

void event_log::add_event(const std::string& event) {
	m_events.emplace_front(
		m_atlas.create_text(event, 1),
		EVENT_TIME
	);
}
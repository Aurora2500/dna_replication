#pragma once

#include <list>

#include "render/mesh.hpp"
#include "render/text.hpp"

class event {
	rendering::Mesh2D text_mesh;
	float time_left;

	friend class event_log;
};

class event_log {
private:
	std::list<event> m_events;
	rendering::TextAtlas& m_atlas;
	

public:
	event_log(rendering::TextAtlas& atlas);
	void update(float dt);

	void add_event(const std::string& event);
};
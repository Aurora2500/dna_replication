#pragma once

#include <queue>

#include "render/mesh.hpp"
#include "render/text.hpp"

class event {
	rendering::Mesh2D text_mesh;
	float time_left;
};

class event_log {
private:
	std::queue<event> m_events;
	rendering::TextAtlas& m_atlas;
	

public:
	inline event_log(rendering::TextAtlas& atlas) : m_atlas(atlas) {}
	void update(float dt);
};
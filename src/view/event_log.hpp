#pragma once

#include <queue>

#include "render/mesh.hpp"

class event {
	rendering::Mesh2D text_mesh;
	float time_left;
};

class event_log {
private:
	std::queue<event> events;

public:
	void update(float dt);
};
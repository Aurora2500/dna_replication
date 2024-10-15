#pragma once

#include <deque>
#include <utility>
#include <list>
#include <variant>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>

struct bspline {
	std::deque<glm::vec4> points;
	std::deque<glm::vec4> speed;
	float drag;

	void update(float dt);
};

using bspline_pair = std::pair<bspline, bspline>;

using bspline_one_two = std::variant<bspline, bspline_pair>;

struct bspline_network {
	std::list<bspline_one_two> segments;

	void update(float dt);
	void spring_correction();
};
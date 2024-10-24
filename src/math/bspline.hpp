#pragma once

#include <array>
#include <deque>
#include <iterator>
#include <utility>
#include <list>
#include <variant>
#include <vector>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include "util/iter_map.hpp"

glm::vec4 eval_bspline(float t, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4);
glm::vec4 eval_bspline_tangent(float t, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4);

struct bspline {
	std::deque<glm::vec4> points;
	std::deque<glm::vec4> speed;

	void update(float dt);
};

using bspline_pair = std::array<bspline, 2>;

using bspline_one_two = std::variant<bspline, bspline_pair>;

class bspline_network_iterator {
private:
	int m_side;
	std::list<bspline_one_two>::iterator m_segment_iter;
	std::list<bspline_one_two>::iterator m_segment_end;
	std::deque<glm::vec4>::iterator m_point_iter;
public:
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = glm::vec4;
	using pointer = value_type*;
	using reference = value_type&;

	const reference operator*();
	const pointer operator->();
	bspline_network_iterator& operator++();
	bspline_network_iterator operator++(int);
	bool operator==(bspline_network_iterator& other);
	bool operator!=(bspline_network_iterator& other);

	friend class bspline_network_iterable;
};

class bspline_network;

class bspline_network_iterable {
private:
	bspline_network& m_nw;
	int m_side;
public:
	bspline_network_iterable(bspline_network& nw, int side);

	bspline_network_iterator begin();
	bspline_network_iterator end();

	friend bspline_network;
};

struct bspline_network {
	std::list<bspline_one_two> segments;

	bspline_network(unsigned int num_points);

	void update(float dt);
	void spring_correction();

	bspline_network_iterable iter(int side);

	void expand_gap(node<bspline_one_two> expanded, bool ascending);

	node<bspline_one_two> create_gap(int pos);

	void debug_print_segment_lengths();

	glm::vec4 eval(float param, int side);
};
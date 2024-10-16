#include "bspline.hpp"

#include <stdexcept>
#include <iterator>
#include <iostream>

#include <glm/glm.hpp>

const float DRAG = 0.1;

// glm is in column major order, so basis matrix is defined in transposed way.
const glm::mat4 b_spline_basis (
	-1./6., 3./6., -3./6., 1./6.,
	3./6., -6./6., 0./6., 4./6.,
	-3./6., 3./6., 3./6., 1./6.,
	1./6., 0./6., 0./6., 0./6.
);

glm::vec4 eval_bspline(float t, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4) {
	glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1.);
	glm::mat4 M = glm::transpose(glm::mat4(p1, p2, p3, p4));
	return T * b_spline_basis * M;
}

glm::vec4 eval_bspline_tangent(float t, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4) {
	glm::vec4 T = glm::vec4(3*t*t, 2*t, 1., 0.);
	glm::mat4 M = glm::transpose(glm::mat4(p1, p2, p3, p4));
	return T * b_spline_basis * M;
}

void bspline::update(float dt) {
	for (int i = 0; i < points.size(); i++) {
		auto &p = points[i];
		auto &s = speed[i];

		p += s * (dt / 2);
		s += - s * DRAG * dt;
		p += s * (dt / 2);
	}
}

bspline_network::bspline_network(unsigned int num_points)
{
	auto& segment = segments.emplace_back(bspline());
	auto& spline = std::get<0>(segment);
	for (int i = 0; i < num_points; i++) {
		spline.points.push_back(glm::vec4(i, 0, 0, i));
		spline.speed.push_back(glm::vec4());
	}
}

void bspline_network::update(float dt) {
	for (auto &segment: segments) {
		if (segment.index() == 0) {
			std::get<0>(segment).update(dt);
		} else {
			auto &sp = std::get<1>(segment);
			sp[0].update(dt);
			sp[1].update(dt);
		}
	}

	spring_correction();
}

const int CORRECTION_ITERATIONS = 6;
const float TARGET_DIST = 1.;

static void correct_pair(glm::vec4& p1, glm::vec4& p2) {
	auto diff = p2.xyz() - p1.xyz();
	auto actual_dist = glm::length(diff);
	auto correction = glm::normalize(diff) * ((actual_dist - TARGET_DIST) / 2.f);
	p1 += glm::vec4(correction, 0.);
	p2 -= glm::vec4(correction, 0.);
}

struct bspline_corrector {
	void operator()(bspline& spline) {
		for (int i = 0; i < spline.points.size() - 1; i++) {
			auto& p1 = spline.points[i];
			auto& p2 = spline.points[i+1];
			correct_pair(p1, p2);
		}
	}

	void operator()(bspline_pair& pair) {
		(*this)(pair[0]);
		(*this)(pair[1]);
	}
};

void bspline_network::spring_correction() {
	bspline_corrector bc_individual;
	for (int correction = 0; correction < CORRECTION_ITERATIONS; correction++) {
		for (auto &segment : segments) {
			std::visit(bc_individual, segment);
		}
		auto it = segments.begin();
		auto last = it++;
		while (it != segments.end()) {
			auto& left = *last;
			auto& right = *it;
			
			if (left.index() == right.index()) {
				throw std::runtime_error("Two contiguous segments on the network are of the same type");
			}
			if (left.index() == 0) {
				auto& pr = std::get<1>(right);
				auto& pl = std::get<0>(left).points.back();
				auto& pr1 = pr[0].points.front();
				auto& pr2 = pr[1].points.front();
				correct_pair(pl, pr1);
				correct_pair(pl, pr2);
			} else {
				auto& pl = std::get<1>(left);
				auto& pr = std::get<0>(right).points.front();
				auto& pl1 = pl[0].points.back();
				auto& pl2 = pl[1].points.back();
				correct_pair(pr, pl1);
				correct_pair(pr, pl2);
			}

			last = it;
			it++;
		}
	}
}

void bspline_network::expand_gap(node<bspline_one_two> expand, bool ascending) {
	if (!ascending && expand == segments.begin()) return;
	if (ascending && expand == segments.end()) return;
	if (expand->index() != 1) return;
	auto& expanded_splines = std::get<1>(*expand);
	auto expanding_into = (ascending ? std::next(expand) : std::prev(expand));
	if (expanding_into->index() == 1) return;
	auto& spline_reduced = std::get<0>(*expanding_into);
	// if we're ascending, we're yoinking the first point, otherwise the last point
	auto point = (ascending?spline_reduced.points.front() : spline_reduced.points.back());
	auto speed = (ascending?spline_reduced.speed.front() : spline_reduced.speed.back());
	for (int i = 0; i < 2; i++) {
		// if we're ascending, push it to the back, otherwise push it to the front
		if (ascending) {
			expanded_splines[i].points.push_back(point);
		} else {
			expanded_splines[i].points.push_front(point);
		}
	}
	if (ascending) {
		spline_reduced.points.pop_front();
		spline_reduced.speed.pop_front();
	} else {
		spline_reduced.points.pop_back();
		spline_reduced.speed.pop_back();
	}
}

static int bspline_size(bspline_one_two& bs) {
	if (bs.index() == 0) {
		return std::get<0>(bs).points.size();
	} else {
		return std::get<1>(bs)[0].points.size();
	}
}

node<bspline_one_two> bspline_network::create_gap(int pos) {
	auto it = segments.begin();
	int as_far = 0;
	while (it != segments.end() && as_far + bspline_size(*it) < pos) {
		pos += bspline_size(*it);
		it++;
	}
	if (it == segments.end()) throw std::runtime_error("pos outside of spline size");
	auto& target_spline = *it;
	if (target_spline.index() == 1) return it;
	auto& spline_to_split = std::get<0>(target_spline);
	int split_spline_size = bspline_size(*it);
	int pos_in_spline = pos - as_far;
	if (pos_in_spline != 0) {
		auto& unsplit_before = std::get<0>(*segments.insert(it, bspline{}));
		unsplit_before.points.assign(
			spline_to_split.points.begin(),
			std::next(spline_to_split.points.begin(), pos_in_spline)
		);
		spline_to_split.points.erase(
			spline_to_split.points.begin(),
			std::next(spline_to_split.points.begin(), pos_in_spline)
		);
		unsplit_before.speed.assign(
			spline_to_split.speed.begin(),
			std::next(spline_to_split.speed.begin(), pos_in_spline)
		);
		spline_to_split.speed.erase(
			spline_to_split.speed.begin(),
			std::next(spline_to_split.speed.begin(), pos_in_spline)
		);
	}
	auto new_split_spline_it = segments.insert(it, std::variant_alternative_t<1, bspline_one_two>());
	auto& new_split_spline = std::get<1>(*new_split_spline_it);

	for (int i = 0; i < 2; i++) {
		new_split_spline[i].points = {spline_to_split.points.front()};
		new_split_spline[i].speed = {spline_to_split.speed.front()};
	}
	spline_to_split.points.pop_front();
	spline_to_split.speed.pop_front();

	if (spline_to_split.points.size() == 0) {
		segments.erase(it);
	}

	return new_split_spline_it;
}

void bspline_network::debug_print_segment_lengths() {
	for (auto& segment : segments) {
		std::cout << bspline_size(segment) << ' ';
	}
	std::cout << std::endl;
}

bspline_network_iterable bspline_network::iter(int side) {
	return bspline_network_iterable(*this, side);
}

bspline_network_iterable::bspline_network_iterable(bspline_network& nw, int side)
	: m_nw(nw)
	, m_side(side)
{}

bspline_network_iterator bspline_network_iterable::begin() {
	bspline_network_iterator it;
	it.m_side = m_side;
	it.m_segment_iter = m_nw.segments.begin();
	it.m_segment_end = m_nw.segments.end();
	auto& segment = m_nw.segments.front();
	if (segment.index() == 0) {
		it.m_point_iter = std::get<0>(segment).points.begin();
	} else {
		it.m_point_iter = std::get<1>(segment)[m_side].points.begin();
	}
	return it;
}

bspline_network_iterator bspline_network_iterable::end() {
	bspline_network_iterator it;
	it.m_side = m_side;
	it.m_segment_iter = m_nw.segments.end();
	it.m_segment_end = m_nw.segments.end();
	auto& segment = m_nw.segments.back();
	if (segment.index() == 0) {
		it.m_point_iter = std::get<0>(segment).points.end();
	} else {
		it.m_point_iter = std::get<1>(segment)[m_side].points.end();
	}
	return it;
}

const bspline_network_iterator::reference bspline_network_iterator::operator*() {
	return *m_point_iter;
}

const bspline_network_iterator::pointer bspline_network_iterator::operator->() {
	return &*m_point_iter;
}

bspline_network_iterator& bspline_network_iterator::operator++() {
	auto& segment = *m_segment_iter;
	auto& spline = (segment.index() == 0 ?
		std::get<0>(segment) :
		std::get<1>(segment)[m_side]
	);

	m_point_iter++;
	if (m_point_iter == spline.points.end()) {
		m_segment_iter++;
		if (m_segment_iter == m_segment_end) {
			return *this;
		}
		auto& new_segment = *m_segment_iter;
		auto& new_spline = (new_segment.index() == 0 ?
			std::get<0>(new_segment) :
			std::get<1>(new_segment)[m_side]
		);
		m_point_iter = new_spline.points.begin();
	}

	return *this;
}

bspline_network_iterator bspline_network_iterator::operator++(int) {
	auto temp = *this;
	++(*this);
	return temp;
}

bool bspline_network_iterator::operator==(bspline_network_iterator& other) {
	if (m_side != other.m_side) return false;
	if (m_segment_iter != other.m_segment_iter) return false;
	if (m_point_iter != other.m_point_iter) return false;
	return true;
}

bool bspline_network_iterator::operator!=(bspline_network_iterator& other) {
	return !(*this == other);
}

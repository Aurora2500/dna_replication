#include "bspline.hpp"

#include <stdexcept>

#include <glm/glm.hpp>

// glm is in column major order, so basis matrix is defined in transposed way.
const glm::mat4 b_spline_basis (
	-1./6., 3./6., -3./6., 1./6.,
	3./6., -6./6., 0./6., 4./6.,
	-3./6., 3./6., 3./6., 1./6.,
	1./6., 0./6., 0./6., 0./6.
);

void bspline::update(float dt) {
	for (int i = 0; i < points.size(); i++) {
		auto &p = points[i];
		auto &s = speed[i];

		p += s * (dt / 2);
		s += - s * drag * dt;
		p += s * (dt / 2);
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
				auto &pl = std::get<1>(left);
				auto& pr = std::get<0>(right).points.back();
				auto& pl1 = pl[0].points.front();
				auto& pl2 = pl[1].points.front();
				correct_pair(pr, pl1);
				correct_pair(pr, pl2);
			}

			last = it;
			it++;
		}
	}
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

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
			sp.first.update(dt);
			sp.second.update(dt);
		}
	}
}

const int CORRECTION_ITERATIONS = 6;
const float TARGET_DIST = 1.;

static void correct_pair(glm::vec4& p1, glm::vec4& p2) {
	auto diff = p2.xyz() - p1.xyz();
	auto actual_dist = glm::length(diff);
	auto correction = glm::normalize(diff) * ((actual_dist - TARGET_DIST) / 2.f);
	p1 += correction;
	p2 -= correction;
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
		(*this)(pair.first);
		(*this)(pair.second);
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
				auto& pr1 = pr.first.points.front();
				auto& pr2 = pr.second.points.front();
				correct_pair(pl, pr1);
				correct_pair(pl, pr2);
			} else {
				auto &pl = std::get<1>(left);
				auto& pr = std::get<0>(right).points.back();
				auto& pl1 = pl.first.points.front();
				auto& pl2 = pl.second.points.front();
				correct_pair(pr, pl1);
				correct_pair(pr, pl2);
			}

			last = it;
			it++;
		}
	}
}
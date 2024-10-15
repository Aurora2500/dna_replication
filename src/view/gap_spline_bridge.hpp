#pragma once

#include "math/bspline.hpp"
#include "model/strand.hpp"

#include "util/iter_map.hpp"

class gap_spline_bridge {
private:
	iter_map<gap, node<bspline_one_two>> m_bridge;
};


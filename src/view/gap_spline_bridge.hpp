#pragma once

#include "math/bspline.hpp"
#include "model/strand.hpp"
#include "model/proteins.hpp"

#include "util/iter_map.hpp"

class gap_spline_bridge {
private:
	iter_map<gap, node<bspline_one_two>> m_bridge;
	bspline_network& m_network;
public:
	gap_spline_bridge(bspline_network& network);

	void helicase_expansion(Helicase& helicase, float ammount);
	void gap_joining();

	void associate(node<gap> gap_it, node<bspline_one_two> bs_it);
	void disasociate(node<gap> gap_it);
};


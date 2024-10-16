#include "gap_spline_bridge.hpp"

gap_spline_bridge::gap_spline_bridge(bspline_network& network)
	: m_network(network)
{}

void gap_spline_bridge::helicase_expansion(Helicase& helicase, float ammount) {
	auto gap = *helicase.attachment();
	int before = helicase.ascending() ? gap->gap_size.higher : gap->gap_size.lower;
	helicase.expand(ammount);
	int after = helicase.ascending() ? gap->gap_size.higher : gap->gap_size.lower;

	// control points are selected based on whole part,
	// so a change in it means the gap expands in spline space
	if (before != after) {
		m_network.expand_gap(m_bridge[gap], helicase.ascending());
	}
}

void gap_spline_bridge::gap_joining() {

}

void gap_spline_bridge::associate(node<gap> gap_it, node<bspline_one_two> bs_it) {
	m_bridge.emplace(gap_it, bs_it);
}

void gap_spline_bridge::disasociate(node<gap> gap_it) {
	m_bridge.erase(gap_it);
}
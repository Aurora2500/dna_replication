#include "strand.hpp"

#include <algorithm>

bool interval::overlaps(interval other) {
	return higher >= other.lower;
}

Strand::Strand(std::vector<Nucleobase>&& nucleobases)
	: m_nucleobases(nucleobases)
{ }

const std::vector<Nucleobase>& Strand::nucleobases() {
	return m_nucleobases;
}

std::list<gap>& Strand::gaps() {
	return m_gaps;
}

std::list<gap>::iterator Strand::create_gap(float position) {
	auto insert_pos = std::find_if(m_gaps.begin(), m_gaps.end(), [position](gap& g){
		return g.gap_size.lower > position;
	});

	return m_gaps.insert(insert_pos, gap{});
}
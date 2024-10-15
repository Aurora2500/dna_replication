#pragma once

#include <array>
#include <list>
#include <vector>

#include "nucleobase.hpp"

enum class Direction {
	Three = 0,
	Five = 1,
};

struct interval {
	float lower;
	float higher;

	bool overlaps(interval other);
};

struct gap {
	// higher number is towards direction end
	std::array<std::list<interval>, 2> okazaki_fragments;
	interval gap_size;
};

class Strand {
private:
	std::vector<Nucleobase> m_nucleobases;
	std::list<gap> m_gaps;

public:
	Strand(std::vector<Nucleobase>&& nucleobases);

	const std::vector<Nucleobase>& nucleobases();
	std::list<gap>& gaps();

	std::list<gap>::iterator create_gap(float position);
};
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
};

struct gap {
	// higher number is towards direction end
	std::array<std::list<interval>, 2> okazaki_fragments;
	interval gap_size;
};

struct strand_gaps {
	std::list<gap> gaps;
};

class Strand {
private:
	std::vector<Nucleobase> m_nucleobases;
	strand_gaps m_gaps;

public:
	Strand(std::vector<Nucleobase>&& nucleobases);

	const std::vector<Nucleobase>& nucleobases();
	strand_gaps& gaps();
};
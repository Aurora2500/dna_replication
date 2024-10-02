#pragma once

#include <vector>

#include "nucleobase.hpp"

class Strand {
private:
	std::vector<Nucleobase> m_nucleobases;
public:
	Strand(std::vector<Nucleobase>&& nucleobases);

	const std::vector<Nucleobase>& nucleobases();
};
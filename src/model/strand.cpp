#include "strand.hpp"

Strand::Strand(std::vector<Nucleobase>&& nucleobases)
	: m_nucleobases(nucleobases)
{ }

const std::vector<Nucleobase>& Strand::nucleobases() {
	return m_nucleobases;
}
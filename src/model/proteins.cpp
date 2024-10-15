#include "proteins.hpp"

void Helicase::expand(float ammount) {
	auto &gap = **m_attached_gap;

	if (m_ascending) {
		gap.gap_size.higher += ammount;
	} else {
		gap.gap_size.lower -= ammount;
	}
}

void Helicase::attach(bool ascending, std::list<gap>::iterator gap) {
	m_attached_gap = gap;
	m_ascending = ascending;
}

void Helicase::detach() {
	m_attached_gap.reset();
}

std::optional<std::list<gap>::iterator>& Helicase::attachment() {
	return m_attached_gap;
}

bool Helicase::ascending() {
	return m_ascending;
}

void Polymerase::expand(float ammount) {
	auto& fragment = **m_attached_fragment;

	switch (m_direction) {
	case Direction::Three:
		fragment.higher += ammount;
		break;
	case Direction::Five:
		fragment.lower -= ammount;
	}
}

bool Polymerase::is_attached() {
	return m_attached_fragment.has_value();
}
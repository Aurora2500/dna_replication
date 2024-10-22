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

const std::optional<std::list<gap>::iterator>& Helicase::attachment() const {
	return m_attached_gap;
}

bool Helicase::ascending() const {
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

void Polymerase::attach(Direction dir, std::list<interval>::iterator fragment) {
	m_attached_fragment = fragment;
	m_direction = dir;
}

void Polymerase::detach() {
	m_attached_fragment.reset();
}

const std::optional<std::list<interval>::iterator>& Polymerase::attachment() const {
	return m_attached_fragment;
}

Direction Polymerase::direction() {
	return m_direction;
}

bool Ligase::tick(float ammount) {
	m_time -= ammount;
	if (m_time > 0) {
		return false;
	}
	m_time = 0;
	return false;
}

void Ligase::attach(Direction dir, std::list<interval>::iterator fragment, float time) {
	m_attached_fragment = fragment;
	m_direction = dir;
	m_time = time;
}

void Ligase::detach() {
	m_attached_fragment.reset();
}


const std::optional<std::list<interval>::iterator>& Ligase::attachment() const {
	return m_attached_fragment;
}

Direction Ligase::direction() {
	return m_direction;
}
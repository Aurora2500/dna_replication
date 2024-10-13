#pragma once

#include <optional>

#include "strand.hpp"

class Helicase {
private:
	std::optional<std::list<gap>::iterator> m_attached_gap;

public:
};

class Polymerase {
private:
	std::optional<std::list<interval>::iterator> m_attached_fragment;
	Direction m_direction;

public:
	void expand(float ammount);
	bool is_attached();
};
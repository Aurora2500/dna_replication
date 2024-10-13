#include "proteins.hpp"

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
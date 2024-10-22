#pragma once

#include <optional>

#include "strand.hpp"

class Helicase {
private:
	std::optional<std::list<gap>::iterator> m_attached_gap;
	bool m_ascending;

public:
	void expand(float ammount);
	void attach(bool ascending, std::list<gap>::iterator gap);
	void detach();

	const std::optional<std::list<gap>::iterator>& attachment() const;
	bool ascending() const;
};

class Polymerase {
private:
	std::optional<std::list<interval>::iterator> m_attached_fragment;
	Direction m_direction;

public:
	void expand(float ammount);
	void attach(Direction dir, std::list<interval>::iterator fragment);
	void detach();

	const std::optional<std::list<interval>::iterator>& attachment() const;
	Direction direction();
};

class Ligase {
private:
	std::optional<std::list<interval>::iterator> m_attached_fragment;
	Direction m_direction;
	float m_time;

public:
	bool tick(float ammount);
	void attach(Direction dir, std::list<interval>::iterator fragment, float time);
	void detach();

	const std::optional<std::list<interval>::iterator>& attachment() const;
	Direction direction();
};
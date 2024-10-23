#include "camera_controller.hpp"

#include <algorithm>
#include <cmath>

constexpr double sensitivity = 0.01;

CamFocus::CamFocus()
	: kind(FocusObject::StrandPos)
	, pos({0., 0.})
{ }

CamFocus::CamFocus(Helicase* h)
	: kind(FocusObject::Helicase)
	, helicase(h)
{ }

CamFocus::CamFocus(Polymerase* p)
	: kind(FocusObject::Polymerase)
	, polymerase(p)
{ }

CamFocus::CamFocus(std::pair<float, int> p)
	: kind(FocusObject::StrandPos)
	, pos(p)
{ }

CamFocus& CamFocus::operator=(const CamFocus& other) {
	kind = other.kind;
	switch (kind) {
	case FocusObject::StrandPos:
		pos = other.pos;
		break;
	case FocusObject::Helicase:
		helicase = other.helicase;
		break;
	case FocusObject::Polymerase:
		polymerase = other.polymerase;
		break;
	}
	return *this;
}

glm::vec3 CamFocus::focus(bspline_network& bspline) {
	float param;
	int side;
	switch (kind) {
	case FocusObject::StrandPos:
		param = pos.first;
		side = pos.second;
		break;
	case FocusObject::Helicase: {
		auto accending = helicase->ascending();
		auto itvl = (*helicase->attachment())->gap_size;
		param = accending?itvl.higher : itvl.lower;
		side = 0;
	}
		break;
	case FocusObject::Polymerase: {
		auto dir = polymerase->direction();
		side = dir == Direction::Three ? 0 : 1;
		auto itvl = **polymerase->attachment();
		param = dir == Direction::Three? itvl.lower : itvl.higher;
	}
		break;
	}

	return bspline.eval(param, side);
}

CameraController::CameraController(Camera c)
	: cam(c)
	, m_mouse_down(false)
{ }

void CameraController::handle_events(EventManager& events) {
	for (auto& e : events.events()) {
		if (e.kind == EventKind::MouseButton && e.mouse.button == MouseButton::Right) {
			m_mouse_down = e.mouse.pressed;
		}
		if (m_mouse_down && e.kind == EventKind::MouseMotion) {
			cam.yaw() = std::fmod(e.motion.rel.x * sensitivity + cam.yaw(), 2*M_PI);
			cam.pitch() = std::clamp(static_cast<double>(e.motion.rel.y * sensitivity + cam.pitch()), -M_PI_2, M_PI_2);
		}
		if (e.kind == EventKind::MouseWheel) {
			cam.distance() -= e.wheel.scroll;
		}
	}
}

void CameraController::update_focus(bspline_network& bspline) {
	cam.focus() = m_focus.focus(bspline);
}

void CameraController::set_focus(CamFocus f) {
	m_focus = f;
}
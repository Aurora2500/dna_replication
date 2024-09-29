#include "camera_controller.hpp"

#include <algorithm>
#include <cmath>

constexpr double sensitivity = 0.01;

CameraController::CameraController(Camera cam)
	: cam(cam)
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
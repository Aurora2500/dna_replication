#pragma once

#include "render/camera.hpp"
#include "windowing/events.hpp"

class CameraController {
public:
	Camera cam;
private:
	bool m_mouse_down;

public:
	CameraController(Camera cam);

	void handle_events(EventManager& events);

};
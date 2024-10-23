#pragma once

#include "render/camera.hpp"
#include "windowing/events.hpp"
#include "model/proteins.hpp"
#include "math/bspline.hpp"

enum class FocusObject {
	StrandPos,
	Helicase,
	Polymerase,
};

struct CamFocus {
	FocusObject kind;
	union {
		std::pair<float, int> pos;
		Helicase* helicase;
		Polymerase *polymerase;
	};

	CamFocus();
	CamFocus(Helicase* h);
	CamFocus(Polymerase* p);
	CamFocus(std::pair<float, int> p);
	CamFocus& operator=(const CamFocus&);

	glm::vec3 focus(bspline_network& bspline);
};

class CameraController {
public:
	Camera cam;
private:
	bool m_mouse_down;
	CamFocus m_focus;

public:
	CameraController(Camera c);

	void handle_events(EventManager& events);
	void update_focus(bspline_network& bspline);
	void set_focus(CamFocus f);
};
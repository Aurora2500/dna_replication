#include "assets/assets_manager.hpp"

#include "render/mesh.hpp"
#include "render/ssbo.hpp"

#include "view/camera_controller.hpp"

#include "windowing/window.hpp"
#include "windowing/events.hpp"

#include <vector>
#include <glm/glm.hpp>

std::vector<glm::vec3> control_points() {

	int n = 20;

	std::vector<glm::vec3> res;
	res = {
		{-n/2, 0.0, 0.0},
		{-n/2, 0.0, 0.0},
		{-n/2, 0.0, 0.0},
	};

	for (int i = 0; i < n; i++) {
		res.push_back({i-n/2, sin(i), cos(i)});
	}
	res.push_back({n/2, 0.0, 0.0});
	res.push_back({n/2, 0.0, 0.0});
	res.push_back({n/2, 0.0, 0.0});

	return res;
}

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;

	assets::AssetsManager asset_manager;

	CameraController cam(window.aspect());

	auto& backbone_shader = asset_manager.get_shader("backbone");
	auto backbone = rendering::create_backbone_mesh(10, 8, 0.1);
	// auto tri = rendering::create_test_tri();

	auto cp = control_points();
	rendering::SSBO ssbo;
	ssbo.set_data(cp.data(), cp.size() * sizeof(glm::vec3));
	ssbo.bind_shader(0);



	backbone_shader.use();
	while (window.running()) {
		event_manager.poll();
		window.clear();
		cam.handle_events(event_manager);
		auto view_proj = cam.cam.get_view_matrix();
		backbone_shader.set_uniform("vp", view_proj);

		backbone.draw_instanced(cp.size() - 4);
		// tri.draw();

		window.handle_events(event_manager);
		window.update();
	}
}
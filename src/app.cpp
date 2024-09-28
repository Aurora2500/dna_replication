#include "assets/assets_manager.hpp"

#include "render/mesh.hpp"
#include "render/ssbo.hpp"

#include "view/camera_controller.hpp"

#include "windowing/window.hpp"
#include "windowing/events.hpp"

#include <vector>
#include <glm/glm.hpp>

#include <iostream>

std::vector<glm::vec3> control_points() {

	int n = 20;

	std::vector<glm::vec3> res;
	// res = {
	// 	{-n/2, 0.0, 0.0},
	// 	{-n/2, 0.0, 0.0},
	// 	{-n/2, 0.0, 0.0},
	// 	{-n/2, 0.0, 0.0},
	// };

	// for (int i = 0; i < n; i++) {
	// 	res.push_back({i-n/2, 0, 0});
	// }
	// res.push_back({n/2, 0.0, 0.0});
	// res.push_back({n/2, 0.0, 0.0});
	// res.push_back({n/2, 0.0, 0.0});
	// res.push_back({n/2, 0.0, 0.0});

	res.push_back({0, 0, 0});
	res.push_back({0, 0, 0});
	res.push_back({0, 0, 0});
	res.push_back({0, 0, 0});

	for (int i = 0; i < n; i++) {
		res.push_back({i, i, i});
	}

	return res;
}

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;

	assets::AssetsManager asset_manager;

	CameraController cam(window.aspect());

	auto& backbone_shader = asset_manager.get_shader("backbone");
	auto backbone = rendering::create_backbone_mesh(10, 3, 0.02);

	auto cp = control_points();
	rendering::SSBO ssbo;
	ssbo.set_data(cp.data(), cp.size() * sizeof(glm::vec3));
	ssbo.bind_shader(0);

	backbone_shader.use();

	// for (auto e : cp) {
	// 	std::cout << e.x << " " << e.y << " " << e.z << std::endl;
	// }
	while (window.running()) {
		event_manager.poll();
		window.clear();
		cam.handle_events(event_manager);
		auto view_proj = cam.cam.get_view_matrix();
		backbone_shader.set_uniform("vp", view_proj);

		backbone.draw_instanced(12);

		window.handle_events(event_manager);
		window.update();
	}
}
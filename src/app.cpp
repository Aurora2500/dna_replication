#include "assets/assets_manager.hpp"

#include "debug/line.hpp"

#include "render/mesh.hpp"
#include "render/ssbo.hpp"

#include "view/camera_controller.hpp"

#include "windowing/window.hpp"
#include "windowing/events.hpp"

#include <vector>
#include <glm/glm.hpp>

#include <iostream>

std::vector<glm::vec4> control_points() {

	int n = 20;

	std::vector<glm::vec4> res;

	for (int i = 0; i < n; i++) {
		res.push_back({i, sin(i), cos(i), 0});
		// res.push_back({i, i, i, 0});
	}

	return res;
}

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;

	assets::AssetsManager asset_manager;

	CameraController cam(window.aspect());

	auto cp = control_points();

	auto& backbone_shader = asset_manager.get_shader("backbone");
	auto backbone = rendering::create_backbone_mesh(32, 16, 0.2);
	
	auto &line_shader = asset_manager.get_shader("line");
	std::vector<glm::vec4> debug;
	int debug_pres = 100;
	for (int i = 0; i <= debug_pres; i++) {
		debug.push_back({0., 0., float(i)/debug_pres, 0});
	}
	debug::line d_line(debug);

	rendering::SSBO ssbo;
	ssbo.set_data(cp.data(), cp.size() * sizeof(glm::vec4));
	ssbo.bind_shader(0);


	// for (auto e : cp) {
	// 	std::cout << e.x << " " << e.y << " " << e.z << std::endl;
	// }
	while (window.running()) {
		event_manager.poll();
		window.clear();
		cam.handle_events(event_manager);
		auto view_proj = cam.cam.get_view_matrix();

		backbone_shader.use();
		backbone_shader.set_uniform("vp", view_proj);
		backbone.draw_instanced(cp.size() - 3);
		
		line_shader.use();
		line_shader.set_uniform("vp", view_proj);
		line_shader.set_uniform("line_col", glm::vec3(1., 0., 0.));
		d_line.draw(cp.size() - 1);

		window.handle_events(event_manager);
		window.update();
	}
}
#include "assets/assets_manager.hpp"

#include "render/mesh.hpp"

#include "view/camera_controller.hpp"

#include "windowing/window.hpp"
#include "windowing/events.hpp"

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;

	assets::AssetsManager asset_manager;

	CameraController cam(window.aspect());

	auto& backbone_shader = asset_manager.get_shader("backbone");
	auto backbone = rendering::create_backbone_mesh(10, 8, 0.1);
	// auto tri = rendering::create_test_tri();


	backbone_shader.use();
	while (window.running()) {
		event_manager.poll();
		window.clear();
		cam.handle_events(event_manager);
		auto view_proj = cam.cam.get_view_matrix();
		backbone_shader.set_uniform("vp", view_proj);

		backbone.draw();
		// tri.draw();

		window.handle_events(event_manager);
		window.update();
	}
}
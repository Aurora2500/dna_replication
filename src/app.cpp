#include "app.hpp"

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "assets/assets_manager.hpp"
#include "debug/line.hpp"
#include "model/strand.hpp"
#include "render/mesh.hpp"
#include "render/ssbo.hpp"
#include "view/camera_controller.hpp"
#include "view/msaa.hpp"
#include "view/strand_view.hpp"
#include "windowing/window.hpp"
#include "windowing/events.hpp"

constexpr float TARGET_FPS = 80;
constexpr std::chrono::duration<float> TARGET_DURATION (1/TARGET_FPS);

Strand make_test_strand() {
	std::vector<Nucleobase> s;

	for (int i = 0; i < 30; i++) {
		s.push_back(Nucleobase::Adenine);
		s.push_back(Nucleobase::Thymine);
		s.push_back(Nucleobase::Guanine);
		s.push_back(Nucleobase::Cytosine);
	}

	return Strand(std::move(s));
}

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;
	assets::AssetsManager asset_manager;
	CameraController cam(window.aspect());
	glm::mat4 ortho = glm::ortho(0, window.width(), 0, window.height());

	auto s = make_test_strand();

	strand_view sv(std::move(s));
	auto& helicase_mesh = asset_manager.get_model("helicase");
	auto& ligase_mesh = asset_manager.get_model("ligase");
	auto& polymerase_mesh = asset_manager.get_model("polymerase");

	auto& obj_shader = asset_manager.get_shader("object_free");

	glm::mat4 helicase_model(1.);
	glm::mat3 helicase_normal(1.);

	auto& text_shader = asset_manager.get_shader("text");
	auto& atlas = asset_manager.get_atlas("NotoSans");
	auto hw = atlas.create_text("Howdy, there!", 120);

	auto last_time = std::chrono::steady_clock::now();
	while (window.running()) {
		auto current_time = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt_duration = current_time - last_time;
		last_time = current_time;
		float dt = dt_duration.count();

		event_manager.poll();
		window.clear();
		cam.handle_events(event_manager);

		auto vp = cam.cam.get_view_matrix();

		sv.update(dt);
		sv.draw(vp, asset_manager);

		glDisable(GL_DEPTH_TEST);
		text_shader.use();
		text_shader.set_uniform("ortho", glm::ortho(0, 1600, 0, 900));
		text_shader.set_uniform("color", glm::vec3(1));
		text_shader.set_uniform("screen_pos", glm::vec2(200));
		atlas.texture().bind(0);
		hw.draw();
		glEnable(GL_DEPTH_TEST);

		window.handle_events(event_manager);
		window.update();
		// std::cout << "fps: " << 1/dt << std::endl;
		if (dt_duration < TARGET_DURATION) {
			std::chrono::duration<float> left_duration = TARGET_DURATION - dt_duration;
			std::this_thread::sleep_for(left_duration);
		}
	}
}
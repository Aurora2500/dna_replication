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

	for (int i = 0; i < 300; i++) {
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

	auto s = make_test_strand();

	strand_view sv(std::move(s), window.aspect());
	auto last_time = std::chrono::steady_clock::now();
	while (window.running()) {
		auto current_time = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt_duration = current_time - last_time;
		last_time = current_time;
		float dt = dt_duration.count();

		event_manager.poll();
		window.clear();

		sv.handle_events(event_manager);
		sv.update(dt);
		sv.draw(asset_manager);

		window.handle_events(event_manager);
		window.update();
		if (dt_duration < TARGET_DURATION) {
			std::chrono::duration<float> left_duration = TARGET_DURATION - dt_duration;
			std::this_thread::sleep_for(left_duration);
		}
	}
}
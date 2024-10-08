#include "assets/assets_manager.hpp"

#include "debug/line.hpp"

#include "model/strand.hpp"

#include "render/mesh.hpp"
#include "render/ssbo.hpp"

#include "view/camera_controller.hpp"
#include "view/strand_view.hpp"
#include "view/msaa.hpp"

#include "windowing/window.hpp"
#include "windowing/events.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

std::vector<glm::vec4> control_points() {

	int n = 20;

	std::vector<glm::vec4> res;

	for (int i = -n; i < n; i++) {
		res.push_back({i, 0, 0, i});
	}

	return res;
}

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

	auto cp = control_points();
	auto s = make_test_strand();

	strand_view sv(std::move(s), cp);
	auto& helicase_mesh = asset_manager.get_model("helicase");
	auto& ligase_mesh = asset_manager.get_model("ligase");
	auto& polymerase_mesh = asset_manager.get_model("polymerase");

	auto& obj_shader = asset_manager.get_shader("object_free");

	glm::mat4 helicase_model(1.);
	glm::mat3 helicase_normal(1.);

	helicase_model = glm::translate(helicase_model, glm::vec3(0.0, 3., 0.f));
	glm::mat4 ligase_model = glm::translate(helicase_model, glm::vec3(-2., 0., 0.));
	glm::mat4 polymerase_model = glm::translate(helicase_model, glm::vec3(2., 0., 0.));

	MSAA msaa(window.width(), window.height());

	while (window.running()) {
		event_manager.poll();
		//window.clear();
		{
			MSAA_Guard msaa_guard(msaa);
			cam.handle_events(event_manager);

			auto vp = cam.cam.get_view_matrix();

			sv.draw(vp, asset_manager);

			obj_shader.use();
			obj_shader.set_uniform("vp", vp);
			obj_shader.set_uniform("normal_matrix", helicase_normal);

			obj_shader.set_uniform("surface_color", glm::vec3(0.2, 0.7, 0.3));
			obj_shader.set_uniform("model", helicase_model);
			helicase_mesh.draw();

			obj_shader.set_uniform("surface_color", glm::vec3(0.3, 0.3, 0.7));
			obj_shader.set_uniform("model", ligase_model);
			ligase_mesh.draw();

			obj_shader.set_uniform("surface_color", glm::vec3(0.7, 0.2, 0.2));
			obj_shader.set_uniform("model", polymerase_model);
			polymerase_mesh.draw();

			window.handle_events(event_manager);
		}
		window.update();
	}
}
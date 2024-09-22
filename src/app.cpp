#include "windowing/window.hpp"
#include "windowing/events.hpp"

void run_app() {
	Window window("DNA Replication");
	EventManager event_manager;


	while (window.running()) {
		event_manager.poll();
		window.clear();

		window.handle_events(event_manager);
		window.update();
	}
}
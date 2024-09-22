#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "events.hpp"

class Window
{
	SDL_Window *m_window;
	SDL_GLContext m_gl_context;

	bool m_is_fullscreen, m_running;
	int m_width, m_height;

public:
	Window(const char *title);
	~Window();

	void clear();
	void bind_and_clear();
	void update();

	int width() const;
	int height() const;
	float aspect() const;

	inline bool running() const { return m_running; }

	void handle_events(EventManager &event);
};
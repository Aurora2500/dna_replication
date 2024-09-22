#include "events.hpp"

#include <SDL2/SDL.h>

static bool to_event(SDL_Event event, Event &out)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		out.kind = EventKind::Keyboard;
		out.keyboard.key = static_cast<SDL_KeyCode>(event.key.keysym.sym);
		out.keyboard.down = true;
		break;
	case SDL_KEYUP:
		out.kind = EventKind::Keyboard;
		out.keyboard.key = static_cast<SDL_KeyCode>(event.key.keysym.sym);
		out.keyboard.down = true;
	case SDL_MOUSEBUTTONDOWN:
		out.kind = EventKind::MouseButton;
		out.mouse.button = static_cast<MouseButton>(event.button.button);
		out.mouse.pos.x = event.button.x;
		out.mouse.pos.y = event.button.y;
		break;
	case SDL_MOUSEBUTTONUP:
		out.kind = EventKind::MouseButton;
		out.mouse.button = static_cast<MouseButton>(event.button.button);
		out.mouse.pos.x = event.button.x;
		out.mouse.pos.y = event.button.y;
		break;
	case SDL_MOUSEMOTION:
		out.kind = EventKind::MouseMotion;
		out.motion.rel.x = event.motion.xrel;
		out.motion.rel.y = event.motion.yrel;
		break;
	case SDL_QUIT:
		out.kind = EventKind::Quit;
		break;
	default:
		return false;
	}
	return true;
}

void EventManager::poll()
{
	m_events.clear();
	m_flags = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		Event e;
		if (to_event(event, e))
		{
			m_flags |= static_cast<u_int32_t>(e.kind);
			m_events.push_back(e);
		}
	}
}

bool EventManager::test_flag(EventKind kind) const
{
	return (m_flags & static_cast<u_int32_t>(kind)) != 0;
}

const std::vector<Event>& EventManager::events() const {
	return m_events;
}

EventKind operator|(EventKind const& lhs, EventKind const& rhs)
{
	return static_cast<EventKind>(static_cast<u_int32_t>(lhs) | static_cast<u_int32_t>(rhs));
}

EventKind operator&(EventKind const& lhs, EventKind const& rhs)
{
	return static_cast<EventKind>(static_cast<u_int32_t>(lhs) & static_cast<u_int32_t>(rhs));
}

EventKind operator^(EventKind const& lhs, EventKind const& rhs)
{
	return static_cast<EventKind>(static_cast<u_int32_t>(lhs) ^ static_cast<u_int32_t>(rhs));
}

EventKind operator~(EventKind const& op)
{
	return static_cast<EventKind>(~static_cast<u_int32_t>(op));
}

EventKind& operator|=(EventKind& lhs, EventKind const& rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

EventKind& operator&=(EventKind& lhs, EventKind const& rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

EventKind& operator^=(EventKind& lhs, EventKind const& rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

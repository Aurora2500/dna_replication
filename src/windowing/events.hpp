#pragma once

#include <vector>

#include <SDL2/SDL.h>

enum class EventKind: u_int32_t
{
	None = 0,
	Keyboard = 1 << 0,
	MouseButton = 1 << 1,
	MouseMotion = 1 << 2,
	MouseWheel = 1 << 3,
	Quit = 1 << 5,
};

EventKind operator|(EventKind const& lhs, EventKind const& rhs);
EventKind operator&(EventKind const& lhs, EventKind const& rhs);
EventKind operator^(EventKind const& lhs, EventKind const& rhs);
EventKind operator~(EventKind const& op);
EventKind& operator|=(EventKind& lhs, EventKind const& rhs);
EventKind& operator&=(EventKind& lhs, EventKind const& rhs);
EventKind& operator^=(EventKind& lhs, EventKind const& rhs);

enum class MouseButton {
	Left = SDL_BUTTON_LEFT,
	Middle = SDL_BUTTON_MIDDLE,
	Right = SDL_BUTTON_RIGHT,
};

struct Position {
	int x;
	int y;
};

struct MouseEvent {
	MouseButton button;
	Position pos;
	bool pressed;
};

struct MotionEvent {
	Position rel;
};

struct MouseWheel {
	float scroll;
};

struct KeyboardEvent
{
	SDL_KeyCode key;
	bool down;
};

struct Event
{
	EventKind kind;
	union
	{
		MouseEvent mouse;
		MotionEvent motion;
		KeyboardEvent keyboard;
		MouseWheel wheel;
	};
};

class EventManager
{
private:
	std::vector<Event> m_events;
	u_int32_t m_flags;

public:
	EventManager() = default;

	void poll();

	bool test_flag(EventKind kind) const;
	const std::vector<Event>& events() const;
};
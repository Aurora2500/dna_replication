#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "math/plane.hpp"

struct frustum_planes {
	plane near;
	plane far;
	plane top;
	plane left;
	plane bottom;
	plane right;
};

class Camera
{
private:
	glm::vec3 m_focus;
	float m_yaw;
	float m_pitch;
	float m_distance;
	float m_fov;
	float m_near, m_far;
	float m_aspect;

public:
	Camera(float aspect);

	glm::vec3 pos() const;

	glm::mat4 get_view_matrix() const;

	inline float &yaw() { return m_yaw; }
	inline const float &yaw() const { return m_yaw; }
	inline float &pitch() { return m_pitch; }
	inline const float &pitch() const { return m_pitch; }
	inline float &distance() { return m_distance; }
	inline float &fov() { return m_fov; }
	inline const float &fov() const { return m_fov; }
	inline float &aspect() { return m_aspect; }
	inline const float &aspect() const { return m_aspect; }
	inline glm::vec3 &focus() { return m_focus; }
	inline const glm::vec3 &focus() const { return m_focus; }

	inline float near() const { return m_near; }
	inline float far() const { return m_far; }

	frustum_planes frustum() const;
};
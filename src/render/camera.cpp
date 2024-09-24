#include "camera.hpp"

#include <algorithm>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

Camera::Camera(float aspect)
	: m_focus(0.0f, 0.0f, 0.0f)
	, m_yaw(0.0f)
	, m_pitch(0.0f)
	, m_distance(10.0f)
	, m_fov(25.0f)
	, m_near(0.1f)
	, m_far(300.0f)
	, m_aspect(aspect)
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::get_view_matrix() const
{
	glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -m_distance));
	view = glm::rotate(view, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::translate(view, -m_focus);

	glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
	return projection * view;
}

glm::mat4 Camera::get_untranslated_view_matrix() const
{
	glm::mat4 view(1.0f);
	view = glm::rotate(view, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(2.5f*m_fov), m_aspect, m_near, m_far);
	return projection * view;
}

glm::vec3 Camera::pos() const
{
	return m_focus + glm::vec3(
		-m_distance * std::cos(m_pitch) * std::sin(m_yaw),
		m_distance * std::sin(m_pitch),
		m_distance * std::cos(m_pitch) * std::cos(m_yaw));
}
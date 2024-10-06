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
	, m_far(3000.0f)
	, m_aspect(aspect)
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

glm::vec3 Camera::pos() const
{
	return m_focus + glm::vec3(
		-m_distance * std::cos(m_pitch) * std::sin(m_yaw),
		m_distance * std::sin(m_pitch),
		m_distance * std::cos(m_pitch) * std::cos(m_yaw));
}

frustum_planes Camera::frustum() const {
	float width_coeff = tan(m_fov / 2);
	float height_coeff = width_coeff * m_aspect;

	float near_width = width_coeff * m_near;
	float near_height = height_coeff * near_height;
	float far_width = width_coeff * m_far;
	float far_height = height_coeff * m_far;

	glm::mat4 local_to_world(1.0f);
	local_to_world = glm::translate(local_to_world, glm::vec3(0.0f, 0.0f, -m_distance));
	local_to_world = glm::rotate(local_to_world, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	local_to_world = glm::rotate(local_to_world, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	local_to_world = glm::translate(local_to_world, -m_focus);

	glm::vec3 nbl = (local_to_world * glm::vec4(near_width, -near_height, m_near, 1.0));
	glm::vec3 nbr = (local_to_world * glm::vec4(-near_width, -near_height, m_near, 1.0));
	glm::vec3 ntl = (local_to_world * glm::vec4(near_width, near_height, m_near, 1.0));
	glm::vec3 ntr = (local_to_world * glm::vec4(-near_width, near_height, m_near, 1.0));
	glm::vec3 fbl = (local_to_world * glm::vec4(far_width, -far_height, m_far, 1.0));
	glm::vec3 fbr = (local_to_world * glm::vec4(-far_width, -far_height, m_far, 1.0));
	glm::vec3 ftl = (local_to_world * glm::vec4(far_width, far_height, m_far, 1.0));
	// eighth point in the frustum, not neccesary as we can do it all with just seven.
	// glm::vec3 ftr = (local_to_world * glm::vec4(-far_width, far_height, m_far, 1.0));

	return {
		.near   = {.point=nbl, .normal=glm::normalize(glm::cross(ntl-nbl, nbr-nbl))},
		.far    = {.point=fbl, .normal=glm::normalize(glm::cross(fbr-fbl, ftl-fbl))},
		.top    = {.point=ntl, .normal=glm::normalize(glm::cross(ftl-ntl, ntr-ntl))},
		.left   = {.point=nbl, .normal=glm::normalize(glm::cross(fbl-nbl, ntl-nbl))},
		.bottom = {.point=nbl, .normal=glm::normalize(glm::cross(nbr-nbl, fbl-nbl))},
		.right  = {.point=nbr, .normal=glm::normalize(glm::cross(ntr-nbr, fbr-nbr))},
	};
}
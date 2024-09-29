#include "line.hpp"

#include <iostream>

namespace debug {

line::line(const std::vector<glm::vec4>& points) : num_points(points.size()) {
	glCreateVertexArrays(1, &m_vao);
	glCreateBuffers(1, &m_vbo);
	glNamedBufferData(
		m_vbo,
		points.size() * sizeof(glm::vec4),
		points.data(),
		GL_STATIC_DRAW);
	
	glEnableVertexArrayAttrib(m_vao, 0);
	glVertexArrayAttribBinding(m_vao, 0, 0);
	glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(glm::vec4));
}

void line::draw() {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINE_STRIP, 0, num_points);
	glBindVertexArray(0);
}

void line::draw(int count) {
	glBindVertexArray(m_vao);
	glDrawArraysInstanced(GL_LINE_STRIP, 0, num_points, count);
	glBindVertexArray(0);
}

}
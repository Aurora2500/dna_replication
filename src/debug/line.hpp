#pragma once

#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

namespace debug {

class line
{
private:
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int num_points;

public:
	line(const std::vector<glm::vec4>& points);

	void draw();
	void draw(int count);
};

}
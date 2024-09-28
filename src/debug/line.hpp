#pragma once

#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

namespace debug {

class line
{
private:
	std::vector<glm::vec3> m_points;
	unsigned int m_id;

public:
	line(std::vector<glm::vec3> points);

};

}
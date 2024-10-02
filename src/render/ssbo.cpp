#include "ssbo.hpp"

namespace rendering {

SSBO::SSBO() {
	glGenBuffers(1, &m_id);
}

SSBO::~SSBO() {
	glDeleteBuffers(1, &m_id);
}

void SSBO::bind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
}

void SSBO::unbind() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::set_data(void *ptr, unsigned int size) {
	bind();
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, ptr, GL_DYNAMIC_DRAW);
}

void SSBO::bind_shader(int n) {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, n, m_id);
}

}
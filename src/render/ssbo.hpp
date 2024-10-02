#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

namespace rendering {

class SSBO {
private:
	unsigned int m_id;

public:
	SSBO();
	~SSBO();

	void bind();
	void unbind();
	void set_data(void* ptr, unsigned int size);
	void bind_shader(int n);

};

}
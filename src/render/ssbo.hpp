#pragma once

#include <vector>

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
	void allocate_bytes(unsigned int bytes);
	void set_data(void* ptr, unsigned int size);
	void update_data(void* ptr, unsigned int size);
	void bind_shader(int n);

	template <typename T>
	inline void allocate(unsigned int num_elems) {
		allocate_bytes(num_elems * sizeof(T));
	}

	template <typename T>
	inline void set(std::vector<T>& data) {
		set_data(data.data(), data.size() * sizeof(T));
	}

	template <typename T>
	inline void update(std::vector<T>& data) {
		update_data(data.data(), data.size() * sizeof(T));
	}
};

}
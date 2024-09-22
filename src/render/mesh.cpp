#include "mesh.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

namespace rendering
{

void SimpleVertex::attrib(unsigned int vao, unsigned int vbo) {
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(SimpleVertex, position));
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(SimpleVertex));
}

template <typename V>
void Mesh<V>::make_buffers()
{
	glCreateVertexArrays(1, &VAO);

	glCreateBuffers(1, &VBO);
	glNamedBufferData(
			VBO,
			vertices.size() * sizeof(V),
			vertices.data(),
			GL_STATIC_DRAW);

	V::attrib(VAO, VBO);

	glCreateBuffers(1, &EBO);
	glNamedBufferData(
			EBO,
			indices.size() * sizeof(unsigned int),
			indices.data(),
			GL_STATIC_DRAW);
	glVertexArrayElementBuffer(VAO, EBO);
}

template <typename V>
void Mesh<V>::draw() const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

SimpleMesh create_backbone_mesh(int segments, int k, float size) {
	SimpleMesh res;

	res.vertices = std::vector<SimpleVertex>(k * (segments + 1));
	for (int i = 0; i <= segments; i++) {
		float t = static_cast<float>(segments) / i;
		for (int j = 0; j < k; j++) {
			float alpha = 2 * M_PI * j / k;
			res.vertices.push_back({{-sin(alpha)*size, cos(alpha)*size, t}});
		}
	}

	res.indices.reserve(6 * k * segments);
	for (int i = 0; i < segments * 4; i+= 4) {
		for (int f = 0; f < k; f++) {
			res.indices.push_back(i +  f);
			res.indices.push_back(i + (f+1)%k);
			res.indices.push_back(i +  f      + k);

			res.indices.push_back(i + (f+1)%k);
			res.indices.push_back(i + (f+1)%k + k);
			res.indices.push_back(i +  f      + k);
		}
	}

	res.make_buffers();
	return res;
}

}
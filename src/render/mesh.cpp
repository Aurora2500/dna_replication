#include "mesh.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <cstddef>

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

template <typename V>
void Mesh<V>::draw_instanced(int count) const
{
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr, count);
	glBindVertexArray(0);
}

SimpleMesh create_backbone_mesh(int segments, int k, float size) {
	SimpleMesh res;

	res.vertices.reserve(k * (segments + 1));
	for (int i = 0; i <= segments; i++) {
		float t = i / static_cast<float>(segments);
		for (int j = 0; j < k; j++) {
			float alpha = 2 * M_PI * j / k;
			res.vertices.push_back({{-sin(alpha)*size, cos(alpha)*size, t}});
		}
	}

	res.indices.reserve(6 * k * segments);
	for (int i = 0; i < segments * k; i+= k) {
		for (int f = 0; f < k; f++) {
			int top_left = i + f;
			int bottom_left = i + (f+1)%k;
			int top_right = i + f + k;
			int bottom_right = i + (f+1)%k + k;

			res.indices.push_back(top_left);
			res.indices.push_back(bottom_left);
			res.indices.push_back(top_right);

			res.indices.push_back(bottom_left);
			res.indices.push_back(bottom_right);
			res.indices.push_back(top_right);
		}
	}

	res.make_buffers();
	return res;
}

SimpleMesh create_test_tri() {
	SimpleMesh res;

	res.vertices.push_back({{-.5, -.5, 0.}});
	res.vertices.push_back({{.5, -.5, 0.}});
	res.vertices.push_back({{0., .5, 0.}});

	res.indices.push_back(0);
	res.indices.push_back(1);
	res.indices.push_back(2);

	res.make_buffers();
	return res;
}

template class Mesh<SimpleVertex>;

}
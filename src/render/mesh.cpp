#include "mesh.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <cstddef>

namespace rendering
{

void Vertex2D::attrib(unsigned int vao, unsigned int vbo)
{
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, position));
	glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, tex_coords));
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex2D));
}

void SimpleVertex::attrib(unsigned int vao, unsigned int vbo) {
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(SimpleVertex, position));
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(SimpleVertex));
}

void PosNormVertex::attrib(unsigned int vao, unsigned int vbo) {
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(PosNormVertex, position));
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(PosNormVertex, normal));
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(PosNormVertex));
}

template<typename V>
Mesh<V>::Mesh(Mesh&& other)
	: vertices(std::move(other.vertices))
	, indices(std::move(other.indices))
	, VAO(other.VAO)
	, VBO(other.VBO)
	, EBO(other.EBO)
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

template<typename V>
Mesh<V>::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
template class Mesh<PosNormVertex>;
template class Mesh<Vertex2D>;

}
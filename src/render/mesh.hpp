#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace rendering
{

struct SimpleVertex
{
	glm::vec3 position;

	static void attrib(unsigned int vao, unsigned int vbo);
};

struct PosNormVertex
{
	glm::vec3 position;
	glm::vec3 normal;

	static void attrib(unsigned int vao, unsigned int vbo);
};

template <typename V>
struct Mesh
{
	std::vector<V> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;

	void make_buffers();

	void draw() const;
	void draw_instanced(int count) const;
};

using SimpleMesh = Mesh<SimpleVertex>;
using PosNormMesh = Mesh<PosNormVertex>;

SimpleMesh create_test_tri();
}
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

template <typename V>
struct Mesh
{
	std::vector<V> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO, VBO, EBO;

	void make_buffers();

	void draw() const;
};

using SimpleMesh = Mesh<SimpleVertex>;

SimpleMesh create_backbone_mesh(int segments, int k, float size);
SimpleMesh create_test_tri();
}
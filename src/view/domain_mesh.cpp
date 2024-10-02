#include "domain_mesh.hpp"

#include <array>

#include <glm/glm.hpp>

rendering::SimpleMesh create_backbone_mesh(int segments, int k, float size) {
	rendering::SimpleMesh res;

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

rendering::PosNormMesh create_nucleobase_mesh(glm::vec3 size) {
	rendering::PosNormMesh res;

	size.x /= 2;
	size.z /= 2;

	res.vertices =  {
		// front
		{{-size.x, 0., size.z}, {0., 0., 1.}},
		{{size.x, 0., size.z}, {0., 0., 1.}},
		{{size.x, size.y, size.z}, {0., 0., 1.}},
		{{-size.x, size.y, size.z}, {0., 0., 1.}},
		// left
		{{size.x, 0., size.z}, {1., 0., 0.}},
		{{size.x, 0., -size.z}, {1., 0., 0.}},
		{{size.x, size.y, -size.z}, {1., 0., 0.}},
		{{size.x, size.y, size.z}, {1., 0., 0.}},
		// back
		{{size.x, 0., -size.z}, {0., 0., -1.}},
		{{-size.x, 0., -size.z}, {0., 0., -1.}},
		{{-size.x, size.y, -size.z}, {0., 0., -1.}},
		{{size.x, size.y, -size.z}, {0., 0., -1.}},
		//right
		{{-size.x, 0., -size.z}, {-1., 0., 0.}},
		{{-size.x, 0., size.z}, {-1., 0., 0.}},
		{{-size.x, size.y, size.z}, {-1., 0., 0.}},
		{{-size.x, size.y, -size.z}, {-1., 0., 0.}},
		// top
		{{size.x, size.y, -size.z}, {0., 1., 0.}},
		{{-size.x, size.y, -size.z}, {0., 1., 0.}},
		{{-size.x, size.y, size.z}, {0., 1., 0.}},
		{{size.x, size.y, size.z}, {0., 1., 0.}},
	};

	res.indices.reserve(6*5);
	for (int i = 0; i < 5; i++) {
		int face = 4*i;

		int bottom_left = face;
		int bottom_right = face + 1;
		int top_right = face + 2;
		int top_left = face + 3;

		res.indices.push_back(bottom_left);
		res.indices.push_back(bottom_right);
		res.indices.push_back(top_left);

		res.indices.push_back(bottom_right);
		res.indices.push_back(top_right);
		res.indices.push_back(top_left);
	}

	res.make_buffers();
	return res;
}

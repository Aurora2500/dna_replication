#include "obj_reader.hpp"

#include <vector>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <charconv>

#include <glm/glm.hpp>

namespace rendering {

std::size_t key(unsigned int i, unsigned int j) {
	return (static_cast<std::size_t>(i) << 32) | j;
}

void read_obj(std::istream& source, PosNormMesh& mesh) {
	std::vector<glm::vec3> vertex_poss;
	std::vector<glm::vec3> vertex_norms;
	std::unordered_map<std::size_t, unsigned int> element_cache;

	std::string line;
	while (std::getline(source, line)) {
		std::string_view sv(line);
		int type_pos = sv.find(' ');
		if (type_pos == sv.npos) {
			continue;
		}
		auto type = sv.substr(0, type_pos);
		sv.remove_prefix(type_pos + 1);
		if (type == "v") {
			auto& v = vertex_poss.emplace_back();
			auto num_len = sv.find(' ');
			std::from_chars(sv.data(), sv.data() + num_len, v.x);
			sv.remove_prefix(num_len+1);
			num_len = sv.find(' ');
			std::from_chars(sv.data(), sv.data() + num_len, v.y);
			sv.remove_prefix(num_len+1);
			std::from_chars(sv.data(), sv.data() + sv.length(), v.z);
		} else if (type == "vn") {
			auto& v = vertex_norms.emplace_back();
			auto num_len = sv.find(' ');
			std::from_chars(sv.data(), sv.data() + num_len, v.x);
			sv.remove_prefix(num_len+1);
			num_len = sv.find(' ');
			std::from_chars(sv.data(), sv.data() + num_len, v.y);
			sv.remove_prefix(num_len+1);
			std::from_chars(sv.data(), sv.data() + sv.length(), v.z);
		} else if (type == "f") {
			for (int i = 0; i < 3; i++) {
				auto len = sv.find('/');
				unsigned int pos_idx;
				std::from_chars(sv.data(), sv.data() + len, pos_idx);
				sv.remove_prefix(len+2);
				len = sv.find(' ');
				if (len == sv.npos) len = sv.length();
				unsigned int norm_idx;
				std::from_chars(sv.data(), sv.data() + len, norm_idx);
				sv.remove_prefix(len+1);

				auto it = element_cache.find(key(pos_idx, norm_idx));
				if (it != element_cache.end()) {
					// we alerady have the element saved
					mesh.indices.push_back(it->second);
				} else {
					// we do not have the element, push a new vertex, save it to the cache
					auto idx = mesh.vertices.size();
					mesh.vertices.emplace_back(vertex_poss[pos_idx-1], vertex_norms[norm_idx-1]);
					mesh.indices.push_back(idx);
					element_cache.emplace(key(pos_idx, norm_idx), idx);
				}
			}
		}
	}

	mesh.make_buffers();
}



}
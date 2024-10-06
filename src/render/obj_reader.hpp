#pragma once

#include <string>

#include "mesh.hpp"

namespace rendering {

// Wavefront .OBJ file format parser
void read_obj(const std::string& source, PosNormMesh& mesh);

}
#pragma once

#include "render/mesh.hpp"

rendering::SimpleMesh create_backbone_mesh(int segments, int k, float size);
rendering::PosNormMesh create_nucleobase_mesh(glm::vec3 size);
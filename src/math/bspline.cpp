#include "bspline.hpp"

#include <glm/glm.hpp>

// glm is in column major order, so basis matrix is defined in transposed way.
const glm::mat4 b_spline_basis (
	-1./6., 3./6., -3./6., 1./6.,
	3./6., -6./6., 0./6., 4./6.,
	-3./6., 3./6., 3./6., 1./6.,
	1./6., 0./6., 0./6., 0./6.
);
//shader fragment
#version 430 core

layout(std430, binding = 0) buffer ControlPoints {
	vec3 points[];
};

const mat4 bspline_basis = mat4(
	-1./6., 3./6., -3./6., 1./6.,
	3./6., -6./6., 0./6., 4./6.,
	-3./6., 3./6., 3./6., 1./6.,
	1./6., 0./6., 0./6., 0./6.
);

vec3 bspline(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
	vec4 T = vec4(t*t*t, t*t, t, 1.0);
	mat3x4 P = transpose(mat4x3(p1, p2, p3, p4));
	return (T * bspline_basis * P);
}

vec3 bspline_tangent(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
	vec4 T_prime = vec4(3 * t*t, 2*t, 1.0, 0.0);
	mat3x4 P = transpose(mat4x3(p1, p2, p3, p4));
	return normalize(T_prime * bspline_basis * P);
}


//shader vertex
#version 430 core

out vec4 col;

void main() {
	col = vec4(1.0, 0.0, 0.0, 1.0);
}
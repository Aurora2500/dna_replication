//shader vertex
#version 430 core

layout(binding = 0) buffer ControlPoints {
	vec4 points[];
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

layout (location = 0) in vec3 pos;
uniform mat4 vp;

out float t;

void main() {
	vec3 p1 = points[gl_InstanceID + 0].xyz;
	vec3 p2 = points[gl_InstanceID + 1].xyz;
	// vec3 p3 = points[gl_InstanceID + 2].xyz;
	// vec3 p4 = points[gl_InstanceID + 3].xyz;
	
	// vec3 point = bspline(pos.z, p1, p2, p3, p4);

	vec3 point = mix(p1, p2, pos.z);

	gl_Position = vp * vec4(point, 1.);
	t = pos.z;
}


//shader fragment
#version 430 core

uniform vec3 line_col;

in float t;

out vec4 col;

void main() {
	col = vec4(line_col * t, 1.);
}
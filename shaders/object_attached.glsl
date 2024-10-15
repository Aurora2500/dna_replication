//shader vertex
#version 430 core

layout(std430, binding = 0) buffer ControlPoints {
	vec4 points[];
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform float param;
uniform mat4 vp;

out vec3 surface_normal;

// b spline basis matrix, GLSL stores data in column major order
// so it's the transpose of what one'd find on the internet
const mat4 bspline_basis = mat4(
	-1./6., 3./6., -3./6., 1./6.,
	3./6., -6./6., 0./6., 4./6.,
	-3./6., 3./6., 3./6., 1./6.,
	1./6., 0./6., 0./6., 0./6.
);

vec4 bspline(float t, vec4 p1, vec4 p2, vec4 p3, vec4 p4) {
	vec4 T = vec4(t*t*t, t*t, t, 1.0);
	mat4 P = transpose(mat4(p1, p2, p3, p4));
	return (T * bspline_basis * P);
}

vec3 bspline_tangent(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
	vec4 T_prime = vec4(3 * t*t, 2*t, 1.0, 0.0);
	mat3x4 P = transpose(mat4x3(p1, p2, p3, p4));
	return normalize(T_prime * bspline_basis * P);
}

const vec3 up = vec3(0.0, 1.0, 0.0);

void main()
{
	uint param_idx = uint(param);
	float param_t = fract(param);

	vec4 p1 = points[param_idx + 0];
	vec4 p2 = points[param_idx + 1];
	vec4 p3 = points[param_idx + 2];
	vec4 p4 = points[param_idx + 3];

	vec4 point = bspline(param_t, p1, p2, p3, p4);
	float param_rot = point.w;
	vec3 tangent = bspline_tangent(param_t, p1.xyz, p2.xyz, p3.xyz, p4.xyz);
	vec3 unrot_binormal = normalize(cross(up, tangent));
	vec3 binormal = cos(param_rot) * unrot_binormal
		- sin(param_rot) * normalize(cross(tangent, unrot_binormal));
	vec3 b_normal = normalize(cross(tangent, binormal));

	mat3 transform = mat3(
		binormal,
		b_normal,
		tangent
	);

	gl_Position = vp * vec4(transform * pos + point.xyz, 1.0);
	surface_normal = transform * normal;
}

//shader fragment
#version 430 core

uniform vec3 surface_color;

in vec3 surface_normal;
out vec4 color;

const vec3 light = vec3(0.3, 0.9, 0.3);

void main()
{
	float shading = dot(normalize(light), normalize(surface_normal));
	shading = clamp(shading, 0., 1.);

	vec3 col = surface_color * mix(0.2, 0.7, shading);
	
	color = vec4(col, 1.0);
}
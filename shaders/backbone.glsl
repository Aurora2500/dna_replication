//shader vertex
#version 430 core

layout(std430, binding = 0) buffer ControlPoints {
	vec3 points[];
};

layout (location = 0) in vec3 bnp;

out vec3 surface_normal;
out float p;

uniform mat4 vp;

// b spline basis matrix, GLSL stores data in column major order
// so it's the transpose of what one'd find on the internet
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

const vec3 up = vec3(0.0, 1.0, 0.0);

void main()
{
	vec3 p1 = points[gl_InstanceID + 0];
	vec3 p2 = points[gl_InstanceID + 1];
	vec3 p3 = points[gl_InstanceID + 2];
	vec3 p4 = points[gl_InstanceID + 3];

	vec3 point = bspline(bnp.z, p1, p2, p3, p4);
	vec3 tangent = bspline_tangent(bnp.z, p1, p2, p3, p4);
	vec3 binormal = normalize(cross(up, tangent));
	vec3 normal = normalize(cross(tangent, binormal));

	gl_Position = vp * vec4(point + bnp.x * binormal + bnp.y * normal, 1.0);
	surface_normal = normalize(bnp.x * binormal + bnp.y * normal);
	p = bnp.z;
}

//shader fragment
#version 330 core

in vec3 surface_normal;
in float p;

layout (location = 0) out vec4 color;

const vec3 light = vec3(0.3, 0.3, -0.7);

void main()
{
	float light = dot(normalize(surface_normal), normalize(light));
	light = clamp(light, 0.0, 1.0);
	vec3 c = mix(vec3(0.3), vec3(0.8), light);
	// c.r = p;
	color = vec4(c, 1.0);
}
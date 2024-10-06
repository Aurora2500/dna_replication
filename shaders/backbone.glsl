//shader vertex
#version 430 core

layout(std430, binding = 0) buffer ControlPoints {
	vec4 points[];
};

layout (location = 0) in vec3 bnp;

out vec3 surface_normal;
out float p;

uniform mat4 vp;
uniform vec2 offset_pos;
uniform bool complement;

const float PI = 3.1415926535897932384626;

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
	vec4 p1 = points[gl_InstanceID + 0];
	vec4 p2 = points[gl_InstanceID + 1];
	vec4 p3 = points[gl_InstanceID + 2];
	vec4 p4 = points[gl_InstanceID + 3];

	vec4 point = bspline(bnp.z, p1, p2, p3, p4);
	vec3 tangent = bspline_tangent(bnp.z, p1.xyz, p2.xyz, p3.xyz, p4.xyz);

	vec3 unrot_binormal =  normalize(cross(up, tangent));

	float angle = point.w + (complement ? PI : 0);

	vec3 binormal = cos(angle) * unrot_binormal - sin(angle) * normalize(cross(tangent, unrot_binormal));
	vec3 normal = normalize(cross(tangent, binormal));

	gl_Position = vp * vec4(point.xyz + (offset_pos.x + bnp.x) * binormal + (offset_pos.y + bnp.y) * normal, 1.0);
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
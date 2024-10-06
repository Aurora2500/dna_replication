//shader vertex
#version 430 core

uniform mat4 vp;
uniform mat4 model;

uniform mat3 normal_matrix;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out vec3 normal;

void main()
{
	gl_Position = vp * model * vec4(pos, 1.);
	normal = normal_matrix * norm;

}

//shader fragment
#version 430 core

uniform vec3 surface_color;

in vec3 normal;
out vec4 color;

const vec3 light = vec3(0.3, 0.9, 0.3);

void main()
{

	float shading = dot(normalize(light), normalize(normal));
	shading = clamp(shading, 0., 1.);

	vec3 col = surface_color * mix(0.2, 0.7, shading);
	
	color = vec4(col, 1.0);
}
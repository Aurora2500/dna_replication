//shader vertex
#version 430 core

uniform mat4 vp;
uniform mat4 model;

uniform mat3 normal_matrix;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out vec3 transformed_normal;

void main()
{
	gl_Position = vp * model * pos;
	transformed_normal = normal_matrix * norm;

}

//shader fragment
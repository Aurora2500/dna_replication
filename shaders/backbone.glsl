//shader vertex
#version 330 core

layout (location = 0) in vec3 bnp;

void main()
{
	gl_Position = vec4(bnp, 1.0)
}


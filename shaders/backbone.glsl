//shader vertex
#version 330 core

layout (location = 0) in vec3 bnp;

void main()
{
	gl_Position = vec4(bnp.x, bnp.z, bnp.y, 1.0);
}

//shader fragment
#version 330 core

layout (location = 0) out vec4 color;

void main()
{
	color = vec4(1.0, 0.0, 0.0, 1.0);
}


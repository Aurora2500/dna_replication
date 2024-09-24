//shader vertex
#version 330 core

layout (location = 0) in vec3 bnp;

out vec3 normal;

uniform mat4 vp;

void main()
{
	gl_Position = vp * vec4(bnp.xyz, 1.0);
	normal = vec3(bnp.xy, 0.0);
}

//shader fragment
#version 330 core

in vec3 normal;

layout (location = 0) out vec4 color;

const vec3 light = vec3(0.3, 0.3, -0.7);

void main()
{
	float light = dot(normalize(normal), normalize(light));
	light = clamp(light, 0.0, 1.0);
	vec3 c = mix(vec3(0.3), vec3(0.8), light);
	color = vec4(c, 1.0);
}


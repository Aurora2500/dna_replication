//shader vertex
#version 430 core

uniform mat4 ortho;
uniform vec2 screen_pos;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv_in;

out vec2 uv;

void main()
{
	uv = uv_in;
	gl_Position = ortho * vec4(pos + screen_pos, 0, 1);
}

//shader fragment
#version 430 core

uniform vec3 color;
uniform sampler2D sdf;

in vec2 uv;

out vec4 frag_col;

void main()
{
	float d = texture(sdf, uv).r;
	float aaf = fwidth(d);
	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
	frag_col = vec4(color, alpha);
}
#version 460 core
layout(location=0) in vec3 Pos;
layout(location=1) in vec2 tex;
out vec2 fs_tex;
uniform mat4 model;
void main()
{
	fs_tex = tex;
	gl_Position = model * vec4(Pos,1.0f);
}
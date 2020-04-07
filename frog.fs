#version 460 core
in vec2 fs_tex;
out vec4 Frag_color;
uniform sampler2D texture1;
//uniform vec4 color;
void main()
{
	Frag_color = texture(texture1,fs_tex);
//	Frag_color = vec4(0.0f,0.0f,0.4f,1.0f);
}
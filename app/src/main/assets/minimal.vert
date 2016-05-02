//Vertex shader
uniform mat4 mvp;
attribute vec4 vert_pos;
attribute vec2 src_tex_coord;
attribute vec4 fill_color;

varying vec4 vert_color;
varying vec2 dest_tex_coord;

void main()
{
	dest_tex_coord = src_tex_coord;
	vert_color = fill_color;
	vec4 pos = mvp * vert_pos;
	pos = pos / pos.w;
	gl_Position = pos;
}
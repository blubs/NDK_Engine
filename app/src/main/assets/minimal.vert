//Vertex shader
attribute vec4 vert_pos;
attribute vec2 src_tex_coord;
attribute vec4 fill_color;

varying vec4 vert_color;
varying vec2 dest_tex_coord;
void main()
{
	dest_tex_coord = src_tex_coord;
	vert_color = fill_color;
	gl_Position = vert_pos;
}
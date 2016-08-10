//Vertex shader
uniform mat4 mvp;
attribute vec4 vert_pos;
attribute vec2 src_tex_coord;
attribute vec4 fill_color;

uniform vec4 test_color_param;

varying vec4 vert_color;
varying vec2 dest_tex_coord;

void main()
{
	dest_tex_coord = src_tex_coord;
	vert_color = test_color_param * fill_color;
	vec4 temp_pos = vec4(vert_pos.x,vert_pos.y,vert_pos.z,1.0);
	vec4 pos = mvp * temp_pos;
	gl_Position = pos;
}
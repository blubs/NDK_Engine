//Vertex shader
uniform mat4 mvp;
attribute vec4 vert_pos;
attribute vec2 src_tex_coord;

varying vec2 dest_tex_coord;

void main()
{
	dest_tex_coord = src_tex_coord;
	vec4 temp_pos = vec4(vert_pos.x,vert_pos.y,vert_pos.z,1.0);
	gl_Position = mvp * temp_pos;
}
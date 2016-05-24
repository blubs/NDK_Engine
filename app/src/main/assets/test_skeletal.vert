//Vertex shader
uniform mat4 mvp;
attribute vec4 vert_pos;
attribute vec2 src_tex_coord;
attribute vec3 bone_weight;
attribute vec3 bone_index;

uniform vec4 test_color_param;

varying vec4 vert_color;
varying vec2 dest_tex_coord;

uniform mat4 bone[2];

void main()
{
	vec4 temp_pos;
	vec4 pos;
	int index;

	dest_tex_coord = src_tex_coord;
	vert_color = vec4(1.0,1.0,1.0,1.0);
	temp_pos = vec4(vert_pos.x,vert_pos.y,vert_pos.z,1.0) * bone_weight.x;
	index = int(bone_index.x);
	pos = mvp * bone[index] * temp_pos;
	gl_Position = pos;
}
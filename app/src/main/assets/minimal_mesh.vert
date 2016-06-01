uniform mat4 mvp;
attribute vec4 vert_pos;

attribute vec3 bone_weight;
attribute vec3 bone_index;

varying vec4 vert_color;
varying vec2 dest_tex_coord;

uniform mat4 bone[58];

void main()
{
	vert_color = vec4(1.0,1.0,1.0,1.0);
	vert_color = vec4(bone_index.x / 57.0,bone_index.y / 57.0,bone_index.z / 57.0,1.0);
	vec4 bone1;
	vec4 bone2;
	vec4 bone3;
	vec4 pos;
	int index;

	pos = vert_pos;
	pos.w = 1.0;

	index = int(bone_index.x);
	bone1 = bone[index] * pos;
	index = int(bone_index.y);
	bone2 = bone[index] * pos;
	index = int(bone_index.z);
	bone3 = bone[index] * pos;

	//Averaging out the bone weights
	pos = bone1 * bone_weight.x + bone2 * bone_weight.y + bone3 * bone_weight.z;

	gl_Position = mvp * pos;
}


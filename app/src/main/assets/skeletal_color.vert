uniform mat4 mvp;
uniform mat3 m_IT;
attribute vec4 vert_pos;
attribute vec3 vert_nor;

attribute vec3 bone_weight;
attribute vec3 bone_index;

varying vec4 vert_color;
varying vec3 normal;
varying vec2 dest_tex_coord;

uniform mat4 bone[58];
uniform mat3 bone_IT[58];

void main()
{
	vec4 bone1;
	vec4 bone2;
	vec4 bone3;
	vec3 bone1_nor;
	vec3 bone2_nor;
	vec3 bone3_nor;
	vec4 pos;
	int index;

	pos = vert_pos;
	pos.w = 1.0;

	index = int(bone_index.x);
	bone1 = bone[index] * pos;
	bone1_nor = bone_IT[index] * vert_nor;

	index = int(bone_index.y);
	bone2 = bone[index] * pos;
	bone2_nor = bone_IT[index] * vert_nor;

	index = int(bone_index.z);
	bone3 = bone[index] * pos;
	bone3_nor = bone_IT[index] * vert_nor;

	//Averaging out the bone weights
	pos = bone1 * bone_weight.x + bone2 * bone_weight.y + bone3 * bone_weight.z;
	normal = m_IT * (bone1_nor * bone_weight.x + bone2_nor * bone_weight.y + bone3_nor * bone_weight.z);
	//normal = m_IT * bone1_nor;//just use the first bone's normal
	//normal = m_IT * vert_nor;//use no bone normals
	vert_color = vec4(0.0,0.0,0.0,1.0);

	gl_Position = mvp * pos;
}


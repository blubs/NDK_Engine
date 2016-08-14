//Actual Shader
uniform mat4 mvp;
uniform mat3 m_IT;
attribute vec4 vert_pos;
attribute vec2 vert_uv;
attribute vec3 vert_nor;
attribute vec3 vert_tan;
attribute vec3 vert_binor;

attribute vec3 bone_weight;
attribute vec3 bone_index;

uniform mat4 bone[58];
uniform mat3 bone_IT[58];

uniform vec3 cam_dir;
uniform vec3 dirlight_dir;

varying vec2 v_uv;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_binor;
varying vec3 cam_dir_tanspace;
varying vec3 dirlight_dir_tanspace;

void main()
{
	vec4 bone1;
	vec4 bone2;
	vec4 bone3;
	mat3 bone1_nor;
	mat3 bone2_nor;
	mat3 bone3_nor;
	vec4 pos;
	int index;

	pos = vec4(vert_pos.xyz,1.0);

	index = int(bone_index.x);
	bone1 = bone[index] * pos;
	bone1_nor = bone_IT[index];

	index = int(bone_index.y);
	bone2 = bone[index] * pos;
	bone2_nor = bone_IT[index];

	index = int(bone_index.z);
	bone3 = bone[index] * pos;
	bone3_nor = bone_IT[index];

	//Averaging out the bone weights
	pos = bone1 * bone_weight.x + bone2 * bone_weight.y + bone3 * bone_weight.z;

	mat3 normal_transform = m_IT * (bone1_nor * bone_weight.x + bone2_nor * bone_weight.y + bone3_nor * bone_weight.z);

	//These 3 vectors are in world space
	vec3 v_nor = normal_transform * vert_nor;
	vec3 v_tan = normal_transform * vert_tan;
	vec3 v_binor = normal_transform * vert_binor;

	mat3 temp = mat3(v_tan,v_binor,v_nor);

	//Manually transposing for what reason? (Does GLES2 not have a transpose function?)
	mat3 world_to_tangent = mat3(temp[0][0],temp[1][0],temp[2][0],
							temp[0][1],temp[1][1],temp[2][1],
							temp[0][2],temp[1][2],temp[2][2]);

	dirlight_dir_tanspace = world_to_tangent * -dirlight_dir;
	cam_dir_tanspace = world_to_tangent * -cam_dir;

	v_uv = vert_uv;
	gl_Position = mvp * pos;
}
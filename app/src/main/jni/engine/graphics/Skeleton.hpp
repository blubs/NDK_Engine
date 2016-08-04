//
// Created by F1 on 5/31/2016.
//

#ifndef ENGINE_SKELETON_H
#define ENGINE_SKELETON_H
#include "../common.hpp"
#include "../File_Utils.hpp"
#include "../Game_Object.hpp"
#include "../misc.hpp"

class Skeleton : public Entity
{
public:
	unsigned int bone_count = 0;
	float* rest_pose;

	float* current_pose_mat4s;
	float* current_pose_mat3s;

	float* rest_pose_ident_mat4s;
	float* rest_pose_ident_mat3s;


	const unsigned int* raw_data = NULL;

	bool lerp_anim = true;
	float frame_time = 1/60.0f;

	int set_fps(float fps);

	//Animation playing end types
	static const int END_TYPE_ROOT_POSE = 0;
	static const int END_TYPE_FREEZE = 1;
	static const int END_TYPE_LOOP = 2;
	static const int END_TYPE_DEFAULT_ANIM = 3;

	bool playing_anim = false;
	int current_frame = 0;
	int dest_frame = 0;
	int current_anim = -1;
	int current_anim_end_type = END_TYPE_ROOT_POSE;
	float time_for_next_frame;

	//Arrays of pointers or values for all animations
	const unsigned int** all_anims_raw_data = NULL;
	unsigned int* anim_lengths;
	//Array of transform 4x4 matrices representing all animation frame transformations
	float** anims;
	//Array of transform 3x3 rotation matrices representing all inverse-transpose of bone transform
	float** anims_IT;

	int anim_count = 0;

	bool playing_default_anim = false;
	int default_anim = -1;
	int default_anim_end_type = END_TYPE_ROOT_POSE;
	//TODO animation blending


	int set_default_anim(int anim, int end_type);

	int anim_is_valid(int anim);

	int play_anim(int anim, int end_type);

	int play_default_anim();

	int stop_anim();

	//Ran every frame to update animation frame logic (calculate interpolation data, increment frame, etc)
	int update_frame();

	void calc_lerped_pose_mats();
	void calc_pose_mats();

	//Returns a pointer to the current frame matrices
	float* get_current_pose();

	//Returns a pointer to the current frame inverse-transpose matrices
	float* get_current_pose_IT();

	//Returns the ith bone's current transform matrix (within animation)
	Mat4 get_bone_transform(int i);
	//Returns the rest transform of the ith bone
	Mat4 get_bone_rest_transform(int i);

	int load_animation(const char* filepath);

	Skeleton(const char* filepath);
	~Skeleton();

private:
	int load(const char* filepath);
	void unload();


};


//Helper class for parenting an Entity to a skeleton bone
class Entity_Bone_Joint : public Game_Object
{
public:
	Skeleton* parent_skel = NULL;
	int parent_bone_index = 0;

	Mat4 get_world_transform(bool modify_trans);
};



#endif //ENGINE_SKELETON_H

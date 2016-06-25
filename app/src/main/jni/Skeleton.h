//
// Created by F1 on 5/31/2016.
//

#ifndef ENGINE_SKELETON_H
#define ENGINE_SKELETON_H
#include "common.h"
#include "File_Utils.h"
#include "Game_Object.h"
#include "misc.h"

class Skeleton : public Entity
{
public:
	unsigned int bone_count = 0;
	float* rest_pose;

	float* rest_pose_ident_mat4s;

	const unsigned int* raw_data = NULL;


	//Animation playing end types
	static const int END_TYPE_ROOT_POSE = 0;
	static const int END_TYPE_FREEZE = 1;
	static const int END_TYPE_LOOP = 2;
	static const int END_TYPE_DEFAULT_ANIM = 3;

	bool playing_anim = false;
	int current_frame = 0;
	//int dest_frame = 0;	TODO: needed later for animation interpolation
	int current_anim = -1;
	int current_anim_end_type = END_TYPE_ROOT_POSE;
	float time_for_next_frame;

	//Arrays of pointers or values for all animations
	const unsigned int** all_anims_raw_data = NULL;
	unsigned int* anim_lengths;
	float** anims;

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

	//Returns a pointer to the current frame matrices
	float* get_current_pose();

	//Returns the ith bone's current transform matrix (within animation)
	Mat4 get_bone_transform(int i);
	//Returns the rest transform of the ith bone
	Mat4 get_bone_rest_transform(int i);

	int load(const char* filepath);
	void unload();

	int load_animation(const char* filepath);

};


//Helper class for parenting an Entity to a skeleton bone
class Entity_Bone_Joint : public Game_Object
{
public:
	Skeleton* parent_skel = NULL;
	int parent_bone_index = 0;

	Mat4 get_world_transform(bool modify_trans)
	{
		if(!parent_skel)
		{
			LOGE("Error: Entity_Bone_Joint skeletal parent not set.\n");
			return Mat4::IDENTITY();
		}
		if(transform_calculated && modify_trans)
		{
			return world_transform;
		}

		transform = parent_skel->get_bone_transform(parent_bone_index) * parent_skel->get_bone_rest_transform(parent_bone_index);

		if(modify_trans)
			transform_calculated = true;
		//Bone transforms seem to introduce a roll of 90 degrees, so undoing it
		Quat fix_roll(HALF_PI, Vec3::FRONT());

		return parent_skel->get_world_transform(modify_trans) * transform * Mat4::ROTATE(fix_roll);
	}
};



#endif //ENGINE_SKELETON_H

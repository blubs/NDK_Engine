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

	//Only support for one animation right now
	//TODO: make it hold an array of animations



	bool playing_anim = false;
	int current_frame = 0;
	//int dest_frame = 0;	TODO: needed later for animation interpolation
	int current_anim = -1;
	float time_for_next_frame;


	unsigned int animcount = 0;

	//Arrays of pointers or values for all animations
	const unsigned int** all_anims_raw_data = NULL;
	unsigned int* anim_lengths;
	float** anims;

	int anim_count = 0;


	int play_anim(int anim)
	{
		if(anim < 0 || anim > anim_count)
		{
			LOGE("Error: tried setting animation to %d when only %d animations are loaded by this skeleton.\n",anim,anim_count);
			return 0;
		}
		current_anim = anim;
		current_frame = 0;
		playing_anim = true;
		return 1;
	}
	int stop_anim()
	{
		playing_anim = false;
		current_anim = -1;
		current_frame = 0;
		return 1;
	}

	//Ran every frame to update animation frame logic (calculate interpolation data, increment frame, etc)
	int update_frame()
	{
		if(!playing_anim)
			return 1;

		float ctime = time();
		if(ctime > time_for_next_frame)
		{
			time_for_next_frame = ctime + 1/30.0f;//30 fps
			current_frame++;
			if(current_frame >= anim_lengths[current_anim])
			{
				current_frame = 0;//looping the animation
			}
		}
		return 1;
	}

	//Returns a pointer to the current frame matrices
	float* get_current_pose()
	{
		if(!playing_anim)
		{
			return rest_pose_ident_mat4s;
		}
		return ((anims[current_anim]) + (bone_count * 16 * current_frame));
	}


	//Returns the ith bone's current transform matrix (within animation)
	Mat4 get_bone_transform(int i)
	{
		if(!playing_anim)
		{
			return Mat4::IDENTITY();
		}
		if(i < 0 || i >= bone_count)
		{
			LOGE("Error: tried getting transform of out of bounds bone index %d, bone_count=%d\n",i,bone_count);
			return Mat4::IDENTITY();
		}

		float* data = ((anims[current_anim]) + (bone_count * 16 * current_frame) + (i*16));
		return Mat4(data);
	}
	//Returns the rest transform of the ith bone
	Mat4 get_bone_rest_transform(int i)
	{
		if(i < 0 || i >= bone_count)
		{
			LOGE("Error: tried getting rest transform of out of bounds bone index %d, bone_count=%d\n",i,bone_count);
			return Mat4::IDENTITY();
		}
		float* data = rest_pose + (i*16);
		return Mat4(data);
	}

	int load(const char* filename)
	{
		raw_data = (unsigned int*) File_Utils::load_raw_asset(filename);
		if(!raw_data)
		{
			LOGE("Error: failed to load \"%s\"\n",filename);
			return 0;
		}

		//File Schematics
		//First int is the bone count
		//List thereafter is a list of matrices in order as they are accessed by the model
		bone_count = raw_data[0];
		rest_pose = (float*) raw_data + 1;

		//Populating a list of identity matrices for displaying the skeleton's rest post
		Mat4 ident = Mat4::IDENTITY();
		rest_pose_ident_mat4s = (float*) malloc(sizeof(float) * bone_count*16);
		for(int i = 0; i < 16; i++)
		{
			for(int j = 0; j < bone_count; j++)
				rest_pose_ident_mat4s[j*16 + i] = ident.m[i];
		}

		return 1;
	}
	void unload()
	{
		if(raw_data)
			free((void*)raw_data);
		if(all_anims_raw_data)
		{
			for(int i = 0; i < anim_count; i++)
			{
				free((void*)all_anims_raw_data[i]);
			}
			free(all_anims_raw_data);

			free(anims);
			free(anim_lengths);
		}
		if(rest_pose_ident_mat4s)
			free(rest_pose_ident_mat4s);
	}

	int load_animation(const char* filename)
	{
		unsigned int* loaded_anim = (unsigned int*) File_Utils::load_raw_asset(filename);

		if(!loaded_anim)
		{
			LOGE("Error: failed to load \"%s\"\n",filename);
			return 0;
		}

		if(loaded_anim[0] != bone_count)
		{
			LOGE("Error: cannot load animation with %ud bones to a skeleton with %ud bones!\n",loaded_anim[0],bone_count);
		}

		anim_count++;

		//Creating new arrays that can hold all of the animations

		const unsigned int** new_anims_data = (const unsigned int**) malloc(sizeof(int*) * anim_count);
		unsigned int* new_anim_lengths = (unsigned int*) malloc(sizeof(unsigned int) * anim_count);
		float** new_anims = (float**) malloc(sizeof(float*) * anim_count);

		if(all_anims_raw_data && anim_lengths && anims)
		{
			//Copying all of the old values
			for(int i = 0; i < anim_count - 1; i++)
			{
				new_anims_data[i] = all_anims_raw_data[i];
				new_anim_lengths[i] = anim_lengths[i];
				new_anims[i] = anims[i];
			}

			//Freeing the old arrays
			free(all_anims_raw_data);
			free(anim_lengths);
			free(anims);
		}
		//Handing off the array location to the class variables
		all_anims_raw_data = new_anims_data;
		anim_lengths = new_anim_lengths;
		anims = new_anims;

		int cindex = anim_count - 1;

		//File Schematics
		//First int is the bone count
		//Second int is the frame count
		//List thereafter is a list of frames
		//Each frame holds a list of matrices per bone
		//animation_bone_count = animation_raw_data[0];


		//Assigning the new just-loaded stuff
		all_anims_raw_data[cindex] = loaded_anim;
		anim_lengths[cindex] = all_anims_raw_data[cindex][1];
		anims[cindex] = (float*) all_anims_raw_data[cindex] + 2;

		return 1;
	}

};


//Helper class for parenting an Entity to a skeleton bone
class Entity_Bone_Joint : public Game_Object
{
public:
	Skeleton* parent_skel = NULL;
	int parent_bone_index = 0;

	Mat4 get_world_transform()
	{
		LOGE("Getting cam to joint world transform\n");
		if(!parent_skel)
		{
			LOGE("Error: Entity_Bone_Joint skeletal parent not set.\n");
			return Mat4::IDENTITY();
		}
		if(transform_calculated)
		{
			return world_transform;
		}


		transform = parent_skel->get_bone_transform(parent_bone_index) * parent_skel->get_bone_rest_transform(parent_bone_index);

		transform_calculated = true;
		//Bone transforms seem to introduce a roll of 90 degrees, so undoing it
		Quat fix_roll(HALF_PI, Vec3::FRONT());

		LOGE("FINISHED getting world transform from entity bone to bone\n");
		return parent_skel->get_world_transform() * transform * Mat4::ROTATE(fix_roll);
	}
};



#endif //ENGINE_SKELETON_H

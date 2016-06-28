//
// Created by F1 on 5/31/2016.
//

#include "Skeleton.h"

int Skeleton::set_default_anim(int anim, int end_type)
{
	if(anim < 0 || anim > anim_count)
	{
		LOGE("Error: tried setting animation to %d when only %d animations are loaded by this skeleton.\n",anim,anim_count);
		return 0;
	}
	default_anim = anim;
	default_anim_end_type = end_type;
}

int Skeleton::anim_is_valid(int anim)
{
	if(anim <0 || anim > anim_count)
		return 0;
	return 1;
}

int Skeleton::play_anim(int anim, int end_type)
{
	if(!anim_is_valid(anim))
	{
		LOGE("Error: tried setting animation to %d when only %d animations are loaded by this skeleton.\n",anim,anim_count);
		return 0;
	}
	current_anim_end_type = end_type;
	current_anim = anim;
	current_frame = 0;
	playing_anim = true;
	return 1;
}

int Skeleton::play_default_anim()
{
	if(!anim_is_valid(default_anim))
	{
		LOGE("Error: tried playing invalid default animation %d.",default_anim);
		return 0;
	}
	playing_default_anim = true;
	current_anim = default_anim;
	current_frame = 0;
	current_anim_end_type = default_anim_end_type;
	return 1;
}

int Skeleton::stop_anim()
{
	playing_anim = false;
	current_anim = -1;
	current_anim_end_type = END_TYPE_ROOT_POSE;
	current_frame = 0;
	return 1;
}

//Ran every frame to update animation frame logic (calculate interpolation data, increment frame, etc)
int Skeleton::update_frame()
{
	if(!playing_anim)
		return 1;

	float ctime = time();
	if(ctime > time_for_next_frame)
	{
		time_for_next_frame = ctime + 1/60.0f;//60 fps
		current_frame++;
		if(current_frame >= anim_lengths[current_anim])
		{
			switch(current_anim_end_type)
			{
				case END_TYPE_ROOT_POSE:
				default:
					stop_anim();
					break;
				case END_TYPE_FREEZE:
					current_frame--;
					break;
				case END_TYPE_LOOP:
					current_frame = 0;
					break;
				case END_TYPE_DEFAULT_ANIM:
					if(anim_is_valid(default_anim))
					{
						play_default_anim();
					}
					else
					{
						LOGW("Warning: anim end type of play_default_anim was summoned with an invalid default anim of %d\n",default_anim);
						stop_anim();
					}
					break;
			}
		}
	}
	return 1;
}

//Returns a pointer to the current frame matrices
float* Skeleton::get_current_pose()
{
	if(!playing_anim)
	{
		return rest_pose_ident_mat4s;
	}
	return ((anims[current_anim]) + (bone_count * 16 * current_frame));
}

//Returns a pointer to the current frame's inverse-transpose matrices
float* Skeleton::get_current_pose_IT()
{
	if(!playing_anim)
	{
		return rest_pose_ident_mat3s;
	}
	return ((anims[current_anim]) + (bone_count * 16 * current_frame));
}


//Returns the ith bone's current transform matrix (within animation)
Mat4 Skeleton::get_bone_transform(int i)
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
int Skeleton::load(const char* filepath)
{
	raw_data = (unsigned int*) File_Utils::load_raw_asset(filepath);
	if(!raw_data)
	{
		LOGE("Error: failed to load \"%s\"\n",filepath);
		return 0;
	}

	//File Schematics
	//First int is the bone count
	//List thereafter is a list of matrices in order as they are accessed by the model
	bone_count = raw_data[0];
	rest_pose = (float*) raw_data + 1;

	//Populating a list of identity matrices for displaying the skeleton's rest post
	Mat4 ident4 = Mat4::IDENTITY();
	rest_pose_ident_mat4s = (float*) malloc(sizeof(float) * bone_count*16);
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < bone_count; j++)
			rest_pose_ident_mat4s[j*16 + i] = ident4.m[i];
	}
	Mat3 ident3 = Mat3::IDENTITY();
	rest_pose_ident_mat3s = (float*) malloc(sizeof(float) * bone_count * 9);
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < bone_count; j++)
			rest_pose_ident_mat3s[j*9 + i] = ident3.m[i];
	}

	return 1;
}

//Returns the rest transform of the ith bone
Mat4 Skeleton::get_bone_rest_transform(int i)
{
	if(i < 0 || i >= bone_count)
	{
		LOGE("Error: tried getting rest transform of out of bounds bone index %d, bone_count=%d\n",i,bone_count);
		return Mat4::IDENTITY();
	}
	float* data = rest_pose + (i*16);
	return Mat4(data);
}
void Skeleton::unload()
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
		free(anims_IT);
		free(anim_lengths);
	}
	if(rest_pose_ident_mat4s)
		free(rest_pose_ident_mat4s);
	if(rest_pose_ident_mat3s)
		free(rest_pose_ident_mat3s);
}

int Skeleton::load_animation(const char* filepath)
{
	unsigned int* loaded_anim = (unsigned int*) File_Utils::load_raw_asset(filepath);

	if(!loaded_anim)
	{
		LOGE("Error: failed to load \"%s\"\n",filepath);
		return 0;
	}

	if(loaded_anim[0] != bone_count)
	{
		LOGE("Error: cannot load animation with %ud bones to a skeleton with %ud bones!\n",loaded_anim[0],bone_count);
		return 0;
	}

	anim_count++;

	//Creating new arrays that can hold all of the animations
	const unsigned int** new_anims_data = (const unsigned int**) malloc(sizeof(int*) * anim_count);
	unsigned int* new_anim_lengths = (unsigned int*) malloc(sizeof(unsigned int) * anim_count);
	float** new_anims = (float**) malloc(sizeof(float*) * anim_count);
	float** new_anims_IT = (float**) malloc(sizeof(float*) * anim_count);

	if(all_anims_raw_data && anim_lengths && anims && anims_IT)
	{
		//Copying all of the old values
		for(int i = 0; i < anim_count - 1; i++)
		{
			new_anims_data[i] = all_anims_raw_data[i];
			new_anim_lengths[i] = anim_lengths[i];
			new_anims[i] = anims[i];
			new_anims_IT[i] = anims_IT[i];
		}
		//Freeing the old arrays
		free(all_anims_raw_data);
		free(anim_lengths);
		free(anims);
		free(anims_IT);
	}
	//Handing off the array location to the class variables
	all_anims_raw_data = new_anims_data;
	anim_lengths = new_anim_lengths;
	anims = new_anims;
	anims_IT = new_anims_IT;

	int cindex = anim_count - 1;

	//File Schematics
	//First int is the bone count
	//Second int is the frame count
	//List thereafter is a list of frames
	//Each frame holds a list of matrices per bone
	//List thereafter is a list of frames
	//Each frame in this list holds the inverse transpose 3x3 Matrix of the bone matrices
	//animation_bone_count = animation_raw_data[0];

	//Assigning the new just-loaded stuff
	all_anims_raw_data[cindex] = loaded_anim;
	anim_lengths[cindex] = all_anims_raw_data[cindex][1];
	anims[cindex] = (float*) all_anims_raw_data[cindex] + 2;
	anims_IT[cindex] = (float*) all_anims_raw_data[cindex] + 2 + (16 * bone_count * anim_lengths[cindex]);
	return 1;
}
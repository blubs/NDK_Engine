//
// Created by F1 on 5/31/2016.
//

#include "Skeleton.hpp"

int Skeleton::set_fps(float fps)
{
	frame_time = 1.0f/fps;
	return 1;
}

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
	dest_frame = 1;
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
	dest_frame = 1;
	current_anim_end_type = default_anim_end_type;
	return 1;
}

int Skeleton::stop_anim()
{
	playing_anim = false;
	current_anim = -1;
	current_anim_end_type = END_TYPE_ROOT_POSE;
	current_frame = 0;
	dest_frame = 0;
	return 1;
}

//Calculate pose matrices somewhere between current_frame and dest_frame
void Skeleton::calc_lerped_pose_mats()
{
	//Getting lerping factor:
	float ctime = time();
	float t = 1.0f - (time_for_next_frame - ctime)/frame_time;
	t = fmaxf(fminf(t,1.0f),0.0f);

	//Repopulating our current frame matrices with the new correct bone matrices
	Mat4 bone_trans;
	Mat3 bone_IT_trans;
	float* trans_a;
	float* trans_b;
	float* trans_IT_a;
	float* trans_IT_b;

	for(int i = 0; i < bone_count; i++)
	{
		//Calculate transformation Mat4
		trans_a = (anims[current_anim]) + (bone_count * 16 * current_frame) + (i*16);
		trans_b = (anims[current_anim]) + (bone_count * 16 * dest_frame) + (i*16);

		bone_trans = Mat4::LERP(Mat4(trans_a),Mat4(trans_b),t);
		//Copying transformation Mat4 into the float array
		for(int j = 0; j < 16; j++)
		{
			current_pose_mat4s[16*i + j] = bone_trans.m[j];
		}

		//Calculate inverse-transpose Mat3
		trans_IT_a = (anims_IT[current_anim]) + (bone_count * 9 * current_frame) + (i*9);
		trans_IT_b = (anims_IT[current_anim]) + (bone_count * 9 * dest_frame) + (i*9);

		bone_IT_trans = Mat3::LERP(Mat3(trans_IT_a),Mat3(trans_IT_b),t);
		//Copying inverse-transpose Mat3 into the float array
		for(int j = 0; j < 9; j++)
		{
			current_pose_mat3s[9*i + j] = bone_IT_trans.m[j];
		}
	}
}

//Calculate at current frame (no lerp)
void Skeleton::calc_pose_mats()
{
	//Repopulating our current frame matrices with the new correct bone matrices
	Mat4 bone_trans;
	Mat3 bone_IT_trans;
	float* trans;
	float* trans_IT;

	for(int i = 0; i < bone_count; i++)
	{
		//Calculate transformation Mat4

		trans = (anims[current_anim]) + (bone_count * 16 * current_frame) + (i*16);
		bone_trans = Mat4(trans);
		//Copying transformation Mat4 into the float array
		for(int j = 0; j < 16; j++)
		{
			current_pose_mat4s[16*i + j] = bone_trans.m[j];
		}


		//Calculate inverse-transpose Mat3
		trans_IT = (anims_IT[current_anim]) + (bone_count * 9 * current_frame) + (i*9);
		bone_IT_trans = Mat3::ROTATE(trans_IT);

		//Copying inverse-transpose Mat3 into the float array
		for(int j = 0; j < 9; j++)
		{
			current_pose_mat3s[9*i + j] = bone_IT_trans.m[j];
		}
	}
}

//Ran every frame to update animation frame logic (calculate interpolation data, increment frame, etc)
int Skeleton::update_frame()
{
	if(!playing_anim)
		return 1;

	float ctime = time();
	if(ctime > time_for_next_frame)
	{
		current_frame += 1;
		dest_frame += 1;
		time_for_next_frame = ctime + frame_time;
		if(current_frame >= anim_lengths[current_anim])
		{
			switch(current_anim_end_type)
			{
				case END_TYPE_ROOT_POSE:
				default:
					stop_anim();
					return 1;
				case END_TYPE_FREEZE:
					current_frame--;
					dest_frame = current_frame;
					break;
				case END_TYPE_LOOP:
					current_frame = 0;
					dest_frame = 1;
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
						return 1;
					}
					break;
			}
		}
		//next_frame = current_frame + 1;
		//dest_frame = next_frame + 1;
		//Setting the frame to lerp to
		if(dest_frame >= anim_lengths[current_anim])
		{
			switch(current_anim_end_type)
			{
				case END_TYPE_ROOT_POSE:
				default:
				case END_TYPE_FREEZE:
					dest_frame--;
					break;
				case END_TYPE_LOOP:
					dest_frame = 0;
					break;
				case END_TYPE_DEFAULT_ANIM:
					//Technically next frame is going to be frame 0 of default anim... I sense complications here
					//TODO / FIXME: this isn't lerped correctly
					//We have to handle fading different anims before figuring this out
					//Don't lerp for now. FIXME
					dest_frame = current_frame;//FIXME should be first frame of next anim? or just current frame? (no lerp)
					break;
			}
		}

	}

	if(current_frame != dest_frame)
		calc_lerped_pose_mats();
	else
		calc_pose_mats();

	return 1;
}

//Returns a pointer to the current frame matrix generation data
float* Skeleton::get_current_pose()
{
	if(!playing_anim)
	{
		return rest_pose_ident_mat4s;
	}
	return current_pose_mat4s;
}

//Returns a pointer to the current frame's inverse-transpose matrix generation data
float* Skeleton::get_current_pose_IT()
{
	if(!playing_anim)
	{
		return rest_pose_ident_mat3s;
	}
	return current_pose_mat3s;
}


//Returns the ith bone's current transform matrix generation data (within animation)
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

	float* data = current_pose_mat4s + (i*16);
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
	current_pose_mat4s = (float*) malloc(sizeof(float) * bone_count*16);
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < bone_count; j++)
		{
			rest_pose_ident_mat4s[j*16 + i] = ident4.m[i];
			//Initializing current pose mat4s to identity matrices
			current_pose_mat4s[j*16 + i] = ident4.m[i];
		}
	}
	Mat3 ident3 = Mat3::IDENTITY();
	rest_pose_ident_mat3s = (float*) malloc(sizeof(float) * bone_count * 9);
	current_pose_mat3s = (float*) malloc(sizeof(float) * bone_count * 9);
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < bone_count; j++)
		{
			rest_pose_ident_mat3s[j*9 + i] = ident3.m[i];
			//Initializing current pose mat3s to identity matrices
			current_pose_mat3s[j*9 + i] = ident3.m[i];
		}
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
	if(current_pose_mat4s)
		free(current_pose_mat4s);
	if(current_pose_mat3s)
		free(current_pose_mat3s);
}

Skeleton::Skeleton(const char *filepath)
{
	load(filepath);
}

Skeleton::~Skeleton()
{
	unload();
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
	//Each frame holds a list of data to build matrices per bone
		//This data is 16 floats for the 4x4 transform matrix
	//List thereafter is a list of frames
	//Each frame in this list holds the data needed to build inverse transpose 3x3 Matrix of the bone matrices
		//This data is 9 floats for the 3x3 matrix
	//animation_bone_count = animation_raw_data[0];

	//Assigning the new just-loaded stuff
	all_anims_raw_data[cindex] = loaded_anim;
	anim_lengths[cindex] = all_anims_raw_data[cindex][1];
	anims[cindex] = (float*) all_anims_raw_data[cindex] + 2;
	anims_IT[cindex] = (float*) all_anims_raw_data[cindex] + 2 + (16 * bone_count * anim_lengths[cindex]);
	return 1;
}

Mat4 Entity_Bone_Joint::get_world_transform(bool modify_trans)
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
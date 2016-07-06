//
// Created by F1 on 5/31/2016.
//

#include "Skeleton.h"

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

//Ran every frame to update animation frame logic (calculate interpolation data, increment frame, etc)
int Skeleton::update_frame(float TEMP_T)
{
	if(!playing_anim)
		return 1;


	//TEMP TEMP TEMP
	//==========================================================================================================
	//Going to test interpolating between frame 160 and frame 179 of showcase hands animation
	//Repopulating our current frame matrices with the new correct bone matrices
	Mat4 bone_trans;
	Mat3 bone_IT_trans;
	float* v_a;
	float* q_a;

	float* v_b;
	float* q_b;

	//float ctime = time();

	static int cframe = 0;

	static float t = 0.0f;

	t += 0.5f * TEMP_T;

	if(t > 1.0f)
	{
		t = 0.0f;
		cframe++;
	}

	if(cframe >= anim_lengths[current_anim])
		cframe = 0;

	current_frame = cframe;
	if(current_frame >= anim_lengths[current_anim] - 1)
		dest_frame = 0;
	else
		dest_frame = cframe+1;
	//current_frame = 160;
	//dest_frame = 169;


	//Getting lerping factor:
	//float t = TEMP_T;
	//float t = 1.0f - (time_for_next_frame - ctime)/frame_time;
	t = fmaxf(fminf(t,1.0f),0.0f);

	for(int i = 0; i < bone_count; i++)
	{
		v_a = (anims[current_anim]) + (bone_count * 7 * current_frame) + (i*7);
		q_a = v_a + 3;
		v_b = (anims[current_anim]) + (bone_count * 7 * dest_frame) + (i*7);
		q_b = v_b + 3;

		Vec3 v = Vec3::LERP(Vec3(v_a),Vec3(v_b),t);
		//Vec3 v = Vec3(v_a);

		Vec3 v1 = Vec3(v_a);
		Vec3 v2 = Vec3(v_b);
		//if(v1 != v2)
		//{
		//	LOGE("[%d] Vec: (%f,%f,%f)->(%f,%f,%f) = (%f,%f,%f)",i,v1.x,v1.y,v1.z,v2.x,v2.y,v2.z,v.x,v.y,v.z);
		//}

		//Quat q = Quat::SLERP(Quat(q_a),Quat(q_b),t);
		//Trying lerp with quats
		Quat q1 = Quat(q_a);
		Quat q2 = Quat(q_b);

		Quat q;

		q = Quat::SLERP(q1,q2,t);
		q.normalize();

		//if(q1 != q2)
		//	LOGE("    Quat: (%f,%f,%f,%f)->(%f,%f,%f,%f) = (%f,%f,%f,%f) @ (%f)",q1.w,q1.v.x,q1.v.y,q1.v.z,q2.w,q2.v.x,q2.v.y,q2.v.z,q.w,q.v.x,q.v.y,q.v.z,t);

		//q.w = quat_a.w + t*(quat_b.w - quat_a.w);
		//q.v = Vec3::LERP(quat_a.v,quat_b.v,t);
		//q.normalize();

		//if(t <= 0.5f)
		//	q = Quat(q_a);
		//else
		//	q = Quat(q_b);

		//Quat q1(q_a);
		//Quat q2(q_b);
		//Quat q;

		//if(q1 != q2)
		//	q = Quat::SLERP(q1,q2,t);
		//else
		//	q = q1;

		//q.normalize();

		//LOGE("Quat: (%f,%f,%f,%f)->(%f,%f,%f,%f) = (%f,%f,%f,%f) @ (%f)",q1.w,q1.v.x,q1.v.y,q1.v.z,q2.w,q2.v.x,q2.v.y,q2.v.z,q.w,q.v.x,q.v.y,q.v.z,t);
		//Quat q = Quat(q_a);

		//TODO: calculate interpolated v and q for anim interpolation
		bone_trans = Mat4::ROT_TRANS(q,v);
		//Copying this Mat4 into the float array
		for(int j = 0; j < 16; j++)
		{
			current_pose_mat4s[16*i + j] = bone_trans.m[j];
		}

		q_a = (anims_IT[current_anim]) + (bone_count * 4 * current_frame) + (i*4);
		q_b = (anims_IT[current_anim]) + (bone_count * 4 * dest_frame) + (i*4);

		//q = Quat::SLERP(Quat(q_a),Quat(q_b),0.0f);
		q = Quat(q_a);
		bone_IT_trans = Mat3::ROTATE(q);
		//TODO: print q from inverse-transpose and compare it to q from regular transform (might be trivial to calculate)
			//This would allow us to avoid having it in the model file and just calculating the IT q here
		//TODO: calculate interpolated q for anim interpolation
		//Copying this Mat3 into the float array
		for(int j = 0; j < 9; j++)
		{
			current_pose_mat3s[9*i + j] = bone_IT_trans.m[j];
		}
	}
	return 1;
	//==========================================================================================================
	//END TEMP TEMP TEMP

	/*
	float ctime = time();
	//TODO: interpolate between frames
	//TODO: interpolate between fading in and out animations
	int next_frame = current_frame;
	if(ctime > time_for_next_frame)
	{
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
		next_frame = current_frame + 1;
		dest_frame = next_frame;
		//Setting the frame to lerp to
		if(next_frame >= anim_lengths[current_anim])
		{
			switch(current_anim_end_type)
			{
				case END_TYPE_ROOT_POSE:
				default:
				case END_TYPE_FREEZE:
					dest_frame = current_frame;
					break;
				case END_TYPE_LOOP:
					dest_frame = 0;
					break;
				case END_TYPE_DEFAULT_ANIM:
					//Technically next frame is going to be frame 0 of default anim... I sense complications here
					//TODO / FIXME: this isn't lerped correctly
					//We have to handle fading different anims before figuring this out
					//Don't lerp for now. FIXME
					dest_frame = current_frame;
					break;
			}
		}

	}

	//Repopulating our current frame matrices with the new correct bone matrices
	Mat4 bone_trans;
	Mat3 bone_IT_trans;
	float* v;
	float* q;


	//Getting lerping factor:
	float t = 1.0f - (time_for_next_frame - ctime)/frame_time;
	t = fmaxf(fminf(t,1.0f),0.0f);
	//LOGE("t = %f",t);

	for(int i = 0; i < bone_count; i++)
	{
		v = (anims[current_anim]) + (bone_count * 7 * current_frame) + (i*7);
		q = v + 3;

		//TODO: calculate interpolated v and q for anim interpolation
		bone_trans = Mat4::ROT_TRANS(Quat(q),Vec3(v));
		//Copying this Mat4 into the float array
		for(int j = 0; j < 16; j++)
		{
			current_pose_mat4s[16*i + j] = bone_trans.m[j];
		}

		q = (anims_IT[current_anim]) + (bone_count * 4 * current_frame) + (i*4);

		bone_IT_trans = Mat3::ROTATE(Quat(q));
		//TODO: calculate interpolated q for anim interpolation
		//Copying this Mat3 into the float array
		for(int j = 0; j < 9; j++)
		{
			current_pose_mat3s[9*i + j] = bone_IT_trans.m[j];
		}
	}
	current_frame = next_frame;
	return 1;*/
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
		//This data is 7 floats, first 3 are the translation vector, last 4 are the rotation quaternion
	//List thereafter is a list of frames
	//Each frame in this list holds the data needed to build inverse transpose 3x3 Matrix of the bone matrices
		//This data is 4 floats that represent the rotation quaternion
	//animation_bone_count = animation_raw_data[0];

	//Assigning the new just-loaded stuff
	all_anims_raw_data[cindex] = loaded_anim;
	anim_lengths[cindex] = all_anims_raw_data[cindex][1];
	anims[cindex] = (float*) all_anims_raw_data[cindex] + 2;
	anims_IT[cindex] = (float*) all_anims_raw_data[cindex] + 2 + (7 * bone_count * anim_lengths[cindex]);
	return 1;
}
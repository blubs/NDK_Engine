//
// Created by F1 on 5/31/2016.
//

#ifndef ENGINE_SKELETON_H
#define ENGINE_SKELETON_H
#include "common.h"
#include "File_Utils.h"

class Skeleton
{
public:
	unsigned int bone_count = 0;
	float* rest_pose;
	const unsigned int* raw_data = NULL;

	//Only support for one animation right now
	//TODO: make it hold an array of animations
	unsigned int animation_bone_count = 0;
	unsigned int animation_length = 0;
	float* animation;
	const unsigned int* animation_raw_data = NULL;

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
		return 1;
	}
	void unload()
	{
		if(raw_data)
			free((void*)raw_data);
		if(animation_raw_data)
			free((void*)animation_raw_data);
	}

	int load_animation(const char* filename)
	{
		animation_raw_data = (unsigned int*) File_Utils::load_raw_asset(filename);
		if(!animation_raw_data)
		{
			LOGE("Error: failed to load \"%s\"\n",filename);
			return 0;
		}

		//File Schematics
		//First int is the bone count
		//Second int is the frame count
		//List thereafter is a list of frames
		//Each frame holds a list of matrices per bone
		animation_bone_count = animation_raw_data[0];
		animation_length = animation_raw_data[1];

		animation = (float*) animation_raw_data + 2;
		return 1;
	}

};


#endif //ENGINE_SKELETON_H

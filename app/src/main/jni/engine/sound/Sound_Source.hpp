//
// Created by F1 on 6/17/2016.
//

#ifndef ENGINE_SOUND_SOURCE_H
#define ENGINE_SOUND_SOURCE_H

#include "../Game_Object.hpp"
#include "Sound_Sample.hpp"

//class Sound_Source : public Entity
class Sound_Source : public Entity
{
public:
	bool used = false;
	int priority = 0;//the lower the priority, the more important
	float volume = 0.0f;
	//TODO: maybe add priority constant identifiers for standardized values
	int sound_pos = 0;

	Sound_Sample* sound = NULL;
	//TODO:need a variable for end style

	//Need duplicate parameters for each audio effect
	float last_falloff_L = 0.0f;
	float last_falloff_R = 0.0f;

};

#endif //ENGINE_SOUND_SOURCE_H

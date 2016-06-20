//
// Created by F1 on 6/17/2016.
//

#ifndef ENGINE_SOUND_SOURCE_H
#define ENGINE_SOUND_SOURCE_H

#include "Game_Object.h"
#include "Sound_Sample.h"

class Sound_Source : public Entity
{
public:
	bool used = false;
	int priority = 0;//the lower the priority, the more important
	float volume = 0.0f;
	//TODO: maybe add priority constant identifiers for standardized values

	Sound_Sample* sound = NULL;
	//TODO:need a variable for end style

	//FIXME: How do we use these from within the object that calls them, AND know how to find them from the engine?
	//FIXME: I sense hardcoding inbound..., though a general solution would be nice

};

#endif //ENGINE_SOUND_SOURCE_H

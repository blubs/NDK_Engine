//
// Created by F1 on 6/17/2016.
//

#ifndef ENGINE_SOUND_SOURCE_H
#define ENGINE_SOUND_SOURCE_H

#include "Game_Object.h"
#include "Sound_Sample.h"

class Sound_Source : public Entity
{
	bool used = false;
	int priority = 0;//the lower the priority, the more important
	//TODO: maybe add priority identifier ints for standardized values like enums

	Sound_Sample* sound;

	//FIXME: How do we use these from within the object that calls them, AND know how to find them from the engine?
	//FIXME: I sense hardcoding inbound..., though a general solution would be nice
};

#endif //ENGINE_SOUND_SOURCE_H

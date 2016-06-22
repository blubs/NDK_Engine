//
// Created by F1 on 6/3/2016.
//

#include "Game_Object.h"
//Placing include here to avoid circular dependency
#include "Audio_Engine.h"

int Entity::play_sound(Sound_Sample* sample)
{
	if(!sample)
	{
		LOGW("Warning: tried playing sound with null sample");
		return 1;
	}
	if(sample->raw_data == NULL)
	{
		LOGW("Warning: tried playing sound with an uninitialized sample (Sample has null data)");
	}

	Audio_Engine::play_sound(sample,this,Vec3::ZERO(),0,1.0f);

	return 1;
}



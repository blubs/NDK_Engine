//
// Created by F1 on 4/15/2016.
//

#ifndef ENGINE_SL_UTILS_H
#define ENGINE_SL_UTILS_H

#include "common.h"

void test_sound_callback(SLBufferQueueItf aSoundQueue,void* context)
{
	//send the next sound buffer
}

//==== Wrap this elsewhere =====
struct SoundInfo
{
	int mSoundID;
	bool mUsed;
	char mPriority;
	char* mData;

	int mLength;
	int mPosition;
	int mStarted;
};


struct Audio_Engine_Data
{
	SLObjectItf audio_interface;
	SLEngineItf audio_engine;
	SLObjectItf output_mix_object;

	//FIXME msounds should not be here
	SoundInfo mSounds[8];
	SLObjectItf sound_player_object;

	SLPlayItf sound_player;
	SLBufferQueueItf sound_queue;
	SLVolumeItf sound_volume;
};


class SL_Utils
{
	void test_function();

	void test_deinit();
};


#endif //ENGINE_SL_UTILS_H

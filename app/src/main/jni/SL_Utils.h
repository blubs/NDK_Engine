//
// Created by F1 on 4/15/2016.
//

#ifndef ENGINE_SL_UTILS_H
#define ENGINE_SL_UTILS_H

#include "common.h"

//==== Wrap this elsewhere =====
struct Sound
{
//	int mSoundID;
	bool mUsed;
	char mPriority;
	char* mData;

	int mLength;
	int mPosition;
	int mStarted;
};


struct Audio_Engine_Data
{

};


class SL_Utils
{

};


#endif //ENGINE_SL_UTILS_H

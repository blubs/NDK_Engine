//
// Created by F1 on 4/15/2016.
//

#ifndef ENGINE_SL_UTILS_H
#define ENGINE_SL_UTILS_H



struct Stereo_Sample
{
	short l;
	short r;
};

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



//#include "common.h"



class SL_Utils
{

};


#endif //ENGINE_SL_UTILS_H

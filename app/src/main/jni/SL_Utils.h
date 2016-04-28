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

struct Sound_Channel
{
	bool used;
	char* data;

	int length;
	int position;

	//NOTE: we are using headless wav files, so ofs field is not needed
	//int ofs;//offset to data (skip header data) //this should be set to 44 for wav files
	// in case we opt to use this again
	// make sure we use this value when reading the sound data from the audio playing routine.

	//priority
	//started

	//Used for interpolation of effects
	float last_falloff;
};


//#include "common.h"



class SL_Utils
{

};


#endif //ENGINE_SL_UTILS_H

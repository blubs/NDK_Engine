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

	int ofs;//offset to data (skip header data)
	//priority
	//started
};


//#include "common.h"



class SL_Utils
{

};


#endif //ENGINE_SL_UTILS_H

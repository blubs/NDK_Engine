//
// Created by F1 on 6/16/2016.
//

#ifndef ENGINE_SOUND_SAMPLE_H
#define ENGINE_SOUND_SAMPLE_H

#include "../common.hpp"
#include "../File_Utils.hpp"
#include "SL_Utils.hpp"

class Sound_Sample
{
public:
	const char* raw_data;

	int length = 0;

	int raw_data_length = 0;

	Sound_Sample(const char* filepath)
	{
		load(filepath);
	}
	~Sound_Sample()
	{
		unload();
	};

private:
	int load(const char* filepath)
	{
		//Test audio file loading
		const Asset_File* sound_file_asset = File_Utils::load_asset(filepath);
		raw_data = (char*) sound_file_asset->data;

		if(!raw_data)
		{
			LOGE("Error: failed to load \"%s\"\n",filepath);
			return 0;
		}
		raw_data_length = sound_file_asset->len;
		length = raw_data_length / sizeof(Stereo_Sample);
		//We only needed the sound file asset as a container to get the pointers to the actual data
		free((void*)sound_file_asset);

		//Example loading of test file from engine_main
		//engine.snd_ch.data = (char*)test_sound_file->data;
		//We want the length in sound samples
		//engine.snd_ch.length = (test_sound_file->len)/sizeof(Stereo_Sample);
		//engine.snd_ch.used = false;
		//Only needed the Asset_File to get the length
		//free((void*)test_sound_file);

		return 1;
	}
	void unload()
	{
		if(raw_data)
			free((void*)raw_data);
		raw_data_length = 0;
	}

};



#endif //ENGINE_SOUND_SAMPLE_H

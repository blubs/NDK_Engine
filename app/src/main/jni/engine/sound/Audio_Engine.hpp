//
// Created by F1 on 6/18/2016.
//

#ifndef ENGINE_AUDIO_ENGINE_H
#define ENGINE_AUDIO_ENGINE_H

#include "SL_Utils.hpp"
#include "../common.hpp"
#include "Sound_Source.hpp"
#include "../../entities/Camera.hpp"

class Audio_Engine
{
private:
	int init_sl();
	void term_sl();
public:

	//May be a hack, but hold a static pointer to the audio_engine instance we are using
	static Audio_Engine* instance;

	Audio_Engine();
	~Audio_Engine();

	//OSL Sound Engine Data
	SLObjectItf sl_engine = NULL;
	SLEngineItf sl_engine_interface = NULL;
	SLObjectItf sl_output_mix = NULL;
	SLObjectItf sl_audio_player = NULL;
	SLPlayItf sl_audio_player_interface = NULL;
	SLBufferQueueItf sl_buffer_queue_interface = NULL;
	SLVolumeItf sl_volume_interface = NULL;
	//void sl_buffer_callback(SLBufferQueueItf snd_queue, void* context);
	void start_audio();
	void stop_audio();
	void pause_audio();
	void play_test_sound();

	//Engine Sound Data
	Stereo_Sample audio_buffer1[SND_AUDIO_BUFFER_SIZE];
	Stereo_Sample audio_buffer2[SND_AUDIO_BUFFER_SIZE];
	Stereo_Sample* active_audio_buffer;
	Stereo_Sample* inactive_audio_buffer;

	//List of sound sources
	Sound_Source* sources;

	const int MAX_SOUND_SOURCES = 32;

	const int SOUND_PRIORITY_AMBIENCE = 0;
	const int SOUND_PRIORITY_PLAYER = 1;
	const int SOUND_PRIORITY_PHYSICS = 2;
	const int SOUND_PRIORITY_MISC = 3;

	//TODO: other parameter constants to figure out
	//sound following is going to be done by whether parent pointer is NULL or set
	//Attenuation type would be good (constant, inverse, inverse squared)



	static int play_sound (Sound_Sample* sound,Entity* ent,Vec3 pos,int sound_priority,float volume);
	int play_sound_sample (Sound_Sample* sound,Entity* ent,Vec3 pos,int sound_priority,float volume);

	Camera* listener = NULL;
	static int set_audio_listener(Camera* lstnr);
};

#endif //ENGINE_AUDIO_ENGINE_H

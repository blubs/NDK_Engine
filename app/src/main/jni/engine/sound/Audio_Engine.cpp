//
// Created by F1 on 6/18/2016.
//

#include "Audio_Engine.hpp"

#include "../misc.hpp"

Audio_Engine* Audio_Engine::instance = NULL;

//#define DEBUG_AUDIO_CALLBACK
//Callback for swapping audio buffers
void sl_buffer_callback (SLBufferQueueItf snd_queue, void *c)
{
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 0 (started)");
#endif
	Audio_Engine *e = ((Audio_Engine *) c);
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 1");
#endif

	//Swap the audio buffers
	Stereo_Sample *other_buffer = e->active_audio_buffer;
	e->active_audio_buffer = e->inactive_audio_buffer;
	e->inactive_audio_buffer = other_buffer;
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 2");
#endif

	//Wipe the current audio buffer
	memset(e->active_audio_buffer, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
	//For sound effect interpolation
	//Last value to current value in SND_AUDIO_BUFFER_SIZE.
	//equation: lerped_effect = i*((cur_effect - last_effect)/SND_AUDIO_BUFFER_SIZE) + last_effect

#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 3");
#endif

	Vec3 listener_pos = Vec3::ZERO();
	Vec3 listener_right = Vec3::RIGHT();
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 4");
#endif
	if(e->listener != NULL)
	{
#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 4.1");
#endif
		listener_pos = e->listener->world_transform.get_pos();
		listener_right = e->listener->right;
	}
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 4.2");
#endif

	//Populate the current audio buffer with the whatever sounds that are playing.
	for(int i = 0; i < e->MAX_SOUND_SOURCES; i++)
	{
#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 5");
#endif
		Sound_Source* source = &e->sources[i];
		if(!source->used)
			continue;

		source->transform_calculated = false;

		//Calculating sound world position
		Vec3 pos = (source->get_world_transform(false)).get_pos() - listener_pos;

#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 5.1");
#endif

		float left_falloff;
		float right_falloff;


		float distance_falloff = 1.0f / pos.len();//This is for inverse falloff

		distance_falloff = fminf(1.0f, distance_falloff);
		distance_falloff = fmaxf(0.0f, distance_falloff);
		//TODO: handle constant and inverse square falloff
		right_falloff = pos.normalized() * listener_right;
		//Bringing dot product between 0 and 1
		right_falloff = 0.5f * (right_falloff + 1.0f);
		//Left falloff is inverse of right
		left_falloff = 1.0f - right_falloff;

		right_falloff *= distance_falloff * source->volume;
		left_falloff *= distance_falloff * source->volume;


		float last_left_falloff = source->last_falloff_L;
		float last_right_falloff = source->last_falloff_R;

		source->last_falloff_L = left_falloff;
		source->last_falloff_R = right_falloff;

		//NOTE: This is done because we set the last falloff to be 0 when we first play the sound
		//This led to a smoothing effect as the falloff was lerped in over a whole sample
		//Instead, if we have no falloff, just use the current falloff as the last
		//If we ever run into weird artifacts at the beginning of sound effects,
		//	remove these lines to revert to the subtle fading in effect
		if(last_left_falloff == 0.0f)
			last_left_falloff = left_falloff;
		if(last_right_falloff == 0.0f)
			last_right_falloff = right_falloff;

		float left_falloff_slope = (left_falloff - last_left_falloff) / SND_AUDIO_BUFFER_SIZE;
		float right_falloff_slope = (right_falloff - last_right_falloff) / SND_AUDIO_BUFFER_SIZE;
#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 5.2");
#endif

		//Calculate "distance" falloff
		//Distance emulated between 0 and 50 meters

		//How many samples to copy? Until this buffer is full, or the sound file is over (whichever happens first)
		int smpls_to_copy = SND_AUDIO_BUFFER_SIZE < (source->sound->length - source->sound_pos) ?
							SND_AUDIO_BUFFER_SIZE : (source->sound->length - source->sound_pos);

		for(int j = 0; j < smpls_to_copy; j++)
		{
			//Calculating current lerped falloff
			left_falloff = left_falloff_slope * j + last_left_falloff;
			right_falloff = right_falloff_slope * j + last_right_falloff;

			Stereo_Sample smp = *((Stereo_Sample *) (source->sound->raw_data) + (j + source->sound_pos));
			smp.l *= left_falloff;
			smp.r *= right_falloff;
			e->active_audio_buffer[j].l += smp.l;
			e->active_audio_buffer[j].r += smp.r;
			//FIXME: this will lead to clipping for loud audio sources, need to add sounds using a different method
		}
#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 5.3");
#endif

		source->sound_pos += smpls_to_copy;

		if(source->sound_pos >= source->sound->length)
		{
			source->used = false;
		}
#ifdef DEBUG_AUDIO_CALLBACK
		LOGE("Audio Callback: 5.4");
#endif
	}
	//Send the prepared audio buffer
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 6");
#endif
	(*(snd_queue))->Enqueue(snd_queue, e->active_audio_buffer, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
#ifdef DEBUG_AUDIO_CALLBACK
	LOGE("Audio Callback: 7 (end)");
#endif
}


Audio_Engine::Audio_Engine()
{
	//Initializing the sound sources
	//These require more than the trivial initializer because of function overloading.
	sources = new Sound_Source[MAX_SOUND_SOURCES];

	if(!init_sl())
		LOGE("Error: failed to initialize audio engine!\n");
	instance = this;
}

Audio_Engine::~Audio_Engine()
{
	delete[] sources;
	term_sl();
}

void Audio_Engine::play_test_sound ()
{
	//if(snd_ch.data == NULL)
	//	return;
	//
	//snd_ch.used = true;
	//snd_ch.position = 0;
}

int Audio_Engine::init_sl ()
{
	LOGI("Audio Engine init started\n");
	//=================================== Creating the SL Sound Engine ======================================
	const SLuint32 eng_mix_iid_count = 1;
	const SLInterfaceID eng_mix_iids[] = {SL_IID_ENGINE};
	const SLboolean eng_mix_reqs[] = {SL_BOOLEAN_TRUE};

	SLresult result;
	result = slCreateEngine(&sl_engine, 0, NULL, eng_mix_iid_count, eng_mix_iids, eng_mix_reqs);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("slCreateEngine failed");
		return 0;
	}

	result = (*sl_engine)->Realize(sl_engine, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine Realize failed");
		return 0;
	}

	result = (*sl_engine)->GetInterface(sl_engine, SL_IID_ENGINE, &sl_engine_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine GetInterface failed");
		return 0;
	}
	//======================================================================================================

	//======================================= Creating the Output Mix object ===============================

	const SLuint32 out_mix_iid_count = 0;
	const SLInterfaceID out_mix_iid[] = {};
	const SLboolean out_mix_req[] = {};

	result = (*sl_engine_interface)->CreateOutputMix(sl_engine_interface, &sl_output_mix, out_mix_iid_count, out_mix_iid,
										    out_mix_req);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine CreatOutputMix failed");
		return 0;
	}
	result = (*sl_output_mix)->Realize(sl_output_mix, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine output_mix_object Realize failed");
		return 0;
	}

	//======================================================================================================


	//============================================= Setting up Audio Player ================================

	//============================ Setting up data sources =================================================

	SLDataLocator_AndroidSimpleBufferQueue bufq_loc;
	bufq_loc.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	bufq_loc.numBuffers = 2;

	//Format of the audio data
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = 2;//1 for mono audio, 2 for stereo audio
	format_pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
	format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.channelMask = SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_LEFT;//SL_SPEAKER_FRONT_CENTER for mono audio
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	//Setting up the audio data source input
	SLDataSource audio_source = {&bufq_loc, &format_pcm};
	audio_source.pLocator = &bufq_loc;
	audio_source.pFormat = &format_pcm;

	//Setting up the audio data source output
	SLDataLocator_OutputMix data_locator_out;
	data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	data_locator_out.outputMix = sl_output_mix;

	SLDataSink data_sink;
	data_sink.pLocator = &data_locator_out;
	data_sink.pFormat = NULL;
	//=================================================================================================

	//================================== Creating the Sound player ====================================
	const SLuint32 snd_plyr_iid_count = 3;
	const SLInterfaceID snd_plyr_iids[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean snd_plyr_reqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


	result = (*sl_engine_interface)->CreateAudioPlayer(sl_engine_interface, &sl_audio_player, &audio_source, &data_sink,
											 snd_plyr_iid_count, snd_plyr_iids, snd_plyr_reqs);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine CreateAudioPlayer failed");
		return 0;
	}
	result = (*sl_audio_player)->Realize(sl_audio_player, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player Realize failed");
	}
	//Getting the three interfaces we requested above

	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_PLAY, &sl_audio_player_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface play failed");
		return 0;
	}
	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_BUFFERQUEUE, &sl_buffer_queue_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface buffer queue failed");
		return 0;
	}
	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_VOLUME, &sl_volume_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface volume failed");
		return 0;
	}
	//===========================================================================================================
	//=============== Setting the Buffer Swapping Callback ======================================================
	// called when current buffer is done playing to prepare the next buffer
	result =
	(*sl_buffer_queue_interface)->RegisterCallback(sl_buffer_queue_interface, sl_buffer_callback, this);//(void*)&osl_engine
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_buffer_queue_interface RegisterCallback failed");
		return 0;
	}
	//===========================================================================================================


	//===================================== Everything after this is for testing ================================
	//Filling audio buffers with all 0s
	memset(audio_buffer1, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
	memset(audio_buffer2, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);

	//Putting a test sine wave on the buffers.
	/*for(int i = 0; i < SND_AUDIO_BUFFER_SIZE; i++)
	{
		audio_buffer1[i].l = (short) (15000 * sin(i * 0.4));
		//Setting the second buffer to have a sine wave that begins at the end of the first buffer's
		audio_buffer2[i].l = (short) (15000 * sin((i + SND_AUDIO_BUFFER_SIZE) * 0.4));

		//Giving the right channel a different sine wave
		audio_buffer1[i].r = (short) (15000 * sin(i * 0.6));
		//Setting the second buffer to have a sine wave that begins at the end of the first buffer's
		audio_buffer2[i].r = (short) (15000 * sin((i + SND_AUDIO_BUFFER_SIZE) * 0.6));
	}*/

	//Point to the first buffer
	active_audio_buffer = audio_buffer1;
	inactive_audio_buffer = audio_buffer2;

	//this is where buffer is wiped to all 0's, audio clips are placed in buffer, etc.
	//Send first audio buffer to the audio player
	(*sl_buffer_queue_interface)->Enqueue(sl_buffer_queue_interface, active_audio_buffer, sizeof(short) * SND_AUDIO_BUFFER_SIZE);
	//Swap the audio buffers
	Stereo_Sample *other_buffer = active_audio_buffer;
	active_audio_buffer = inactive_audio_buffer;
	inactive_audio_buffer = other_buffer;

	start_audio();
	LOGI("Audio Engine init finished\n");
	return 1;
}

void Audio_Engine::term_sl ()
{
	stop_audio();
	if(sl_audio_player != NULL)
	{
		SLuint32 sound_player_state;
		(*sl_audio_player)->GetState(sl_audio_player, &sound_player_state);

		if(sound_player_state == SL_OBJECT_STATE_REALIZED)
		{
			(*sl_buffer_queue_interface)->Clear(sl_buffer_queue_interface);
			(*sl_audio_player)->AbortAsyncOperation(sl_audio_player);
			(*sl_audio_player)->Destroy(sl_audio_player);
			sl_audio_player = NULL;
			sl_audio_player_interface = NULL;
			sl_buffer_queue_interface = NULL;
			sl_volume_interface = NULL;
		}
	}
	//Destroying the output mix object
	if(sl_output_mix != NULL)
	{
		(*sl_output_mix)->Destroy(sl_output_mix);
		sl_output_mix = NULL;
	}

	//Destroying the sound engine
	if(sl_engine != NULL)
	{
		(*sl_engine)->Destroy(sl_engine);
		sl_engine = NULL;
		sl_engine_interface = NULL;
	}
}

int Audio_Engine::set_audio_listener (Camera *cam)
{
	if(instance == NULL)
	{
		LOGW("Warning: tried setting an audio listener without an Audio_Engine instance!\n");
		return 0;
	}
	instance->listener = cam;
	return 1;
}

void Audio_Engine::start_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_PLAYING);
}

void Audio_Engine::stop_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_STOPPED);
}

void Audio_Engine::pause_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_PAUSED);
}

int Audio_Engine::play_sound_sample(Sound_Sample* sound_sample,Entity* ent,Vec3 position, int sound_priority, float vol)
{
	if(sound_sample->raw_data == NULL)
	{
		LOGW("Warning: tried playing sound with an uninitialized sample (Sample has null data)");
		return 0;
	}
	Sound_Source* source = NULL;

	for(int i = 0; i < MAX_SOUND_SOURCES; i++)
	{
		if(!sources[i].used)
		{
			source = &(instance->sources[i]);
			break;
		}
	}
	if(!source)
	{
		LOGW("Warning: couldn't play sound, no free sound sources");
		return 0;
	}

	source->used = true;
	source->sound = sound_sample;
	source->sound_pos = 0;
	source->parent = ent;
	source->pos = position;
	source->priority = sound_priority;
	source->volume = vol;

	source->last_falloff_L = 0.0f;
	source->last_falloff_R = 0.0f;

	// if there are no free sound sources...
	//look for a lower priority sound source to override
	//FIXME: this will lead to audio clipping, so consider fading out the previous sound source before fading in the new one?
	//A system of sounds that were playing, and sounds that are going to play would work for this
	//This would entail a double list of sound_sources, but would allow for fading in and out between sounds
	//OR we could avoid this overriding functionality altogether, and opt to not override
	//if there isn't a free sound slot, simply don't play the sound

	return 1;
}

int Audio_Engine::play_sound (Sound_Sample* sound,Entity* ent,Vec3 pos,int sound_priority,float volume)
{
	if(instance == NULL)
	{
		LOGW("Warning: tried to play sound without an Audio_Engine instance!\n");
		return 0;
	}

	return instance->play_sound_sample(sound,ent,pos,sound_priority,volume);
}
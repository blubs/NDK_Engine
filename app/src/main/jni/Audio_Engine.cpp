//
// Created by F1 on 6/18/2016.
//

#include "Audio_Engine.h"

Audio_Engine* Audio_Engine::instance = NULL;

//Callback for swapping audio buffers
void sl_buffer_callback (SLBufferQueueItf snd_queue, void *c)
{
	Audio_Engine *e = ((Audio_Engine *) c);

	//Swap the audio buffers
	Stereo_Sample *other_buffer = e->active_audio_buffer;
	e->active_audio_buffer = e->inactive_audio_buffer;
	e->inactive_audio_buffer = other_buffer;

	//Wipe the current audio buffer
	memset(e->active_audio_buffer, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);

	//For sound effect interpolation
	//Last value to current value in SND_AUDIO_BUFFER_SIZE.
	//equation: lerped_effect = i*((cur_effect - last_effect)/SND_AUDIO_BUFFER_SIZE) + last_effect


	//Populate the current audio buffer with the whatever sounds that are playing.
	if(e->snd_ch.used)
	{
		//float falloff = 1.0f / (50.0f * (e->state.x) / ((float) e->width));
		float falloff = 1.0f;
		falloff = fminf(1.0f, falloff);
		falloff = fmaxf(0.0f, falloff);
		float last_falloff = e->snd_ch.last_falloff;
		e->snd_ch.last_falloff = falloff;

		float faloff_slope = (falloff - last_falloff) / SND_AUDIO_BUFFER_SIZE;
		//Calculate "distance" falloff (our fingers x coordinate)
		//Distance emulated between 0 and 50 meters

		//Need file length, and audio
		int smpls_cp = SND_AUDIO_BUFFER_SIZE < (e->snd_ch.length - e->snd_ch.position) ? SND_AUDIO_BUFFER_SIZE :
		(e->snd_ch.length - e->snd_ch.position);


		for(int i = 0; i < smpls_cp; i++)
		{
			//Calculating current lerped falloff
			falloff = faloff_slope * i + last_falloff;

			Stereo_Sample smp = *((Stereo_Sample *) (e->snd_ch.data) + (i + e->snd_ch.position));
			smp.l *= falloff;
			smp.r *= falloff;
			e->active_audio_buffer[i] = smp;
		}
		e->snd_ch.position += smpls_cp;
		if((e->snd_ch.position) >= e->snd_ch.length)
		{
			e->snd_ch.used = false;
		}
	}
	//Send the prepared audio buffer
	(*(snd_queue))->Enqueue(snd_queue, e->active_audio_buffer, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
}



void Audio_Engine::play_test_sound ()
{
	if(snd_ch.data == NULL)
		return;

	snd_ch.used = true;
	snd_ch.position = 0;
}

int Audio_Engine::init()
{
	if(!init_sl())
		return 0;
	instance = this;

	//Initializing the sound sources
	sources = (Sound_Source*) malloc(sizeof(Sound_Source) * MAX_SOUND_SOURCES);

	//TODO: initialize memory for objects
	//for(int i = 0; i < MAX_SOUND_SOURCES; i++)
	//{
	//}

	return 1;
}

void Audio_Engine::term()
{
	term_sl();
	free(sources);
}

int Audio_Engine::init_sl ()
{

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

	//TODO: iterate through sound structs setting used to false
	//Setting buffers as not in use
	//	for(int i = 0; i < SND_AUDIO_CHANNELS; i++)
	//	{
	//		osl_engine.mSounds[i].mUsed = false;
	//	}
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

int Audio_Engine::play_sound (Sound_Sample* sound,int sound_priority,float volume)
{
	//TODO:
	// iterate through all sources, looking for a free sound source and use that
/*
	for(int i = 0; i < MAX_SOUND_SOURCES; i++)
	{
		if(sources[i].used == false)
		{
			//do something
		}
	}
*/
	// if there are no free sound sources...
		//look for a lower priority sound source to override
		//FIXME: this will lead to audio clipping, so consider fading out the previous sound source before fading in the new one?
		//A system of sounds that were playing, and sounds that are going to play would work for this
			//This would entail a double list of sound_sources, but would allow for fading in and out between sounds
		//OR we could avoid this overriding functionality altogether, and opt to not override
		//if there isn't a free sound slot, simply don't play the sound

	//Variables that need to be set to play a sound
	//if(snd_ch.data == NULL)
	//	return;

	//snd_ch.used = true;
	//snd_ch.position = 0;
	return 1;
}

#include "engine_main.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

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
void android_main(struct android_app *app)
{
	//Making sure glue is not stripped
	app_dummy();

	Engine engine(app);
	//We never leave this scope anyways
	app->userData = &engine;


	//Trying to get files to load.
	File_Utils::asset_mgr = app->activity->assetManager;

	//====================================

#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: ANDROID_MAIN RAN\n");
#endif

	const char* data = File_Utils::read_savedata("test.dat");
	LOGI("Read file: %s.\n",data);
	free((char*)data);
	File_Utils::write_savedata("test.dat");


	//app->activity->instance->getSystemService(Context.AUDIO_SERVICE);
	//For getting framecount,buffer size, and sample rate
	//where to get env from?
//	jclass audioSystem = env->FindClass("android/media/AudioSystem");
//	jmethodID method = env->GetStaticMethodID(audioSystem, "getPrimaryOutputSamplingRate","()I");
//	jint sample_rate = env->CallStaticIntMethod(autioSystem, method);
//	method = env->GetStaticMethodID(audioSystem,"getPrimaryOutputFrameCount","()I");
//	jint buffer_length = env->CallStaticIntMethod(audioSystem, method);

	//================================================= Initialization ========================================
	const SLuint32 engine_mix_iid_count = 1;
	const SLInterfaceID engine_mix_iids[] = {SL_IID_ENGINE};
	const SLboolean engine_mix_reqs[] = {SL_BOOLEAN_TRUE};

	Audio_Engine_Data osl_engine;


	SLresult result;
	result = slCreateEngine(&osl_engine.audio_interface, 0,NULL, engine_mix_iid_count, engine_mix_iids,engine_mix_reqs);
	if(result != SL_RESULT_SUCCESS)
		LOGE("createEngine failed.");

	result = (*osl_engine.audio_interface)->Realize(osl_engine.audio_interface, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		LOGE("osl_engine audio_interface realization failed.");

	result = (*osl_engine.audio_interface)->GetInterface(osl_engine.audio_interface, SL_IID_ENGINE, &osl_engine.audio_engine);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Get OSL_Engine Interface failed.");

	//==================================================

	const SLuint32 output_mix_iid_count = 0;
	const SLInterfaceID output_mix_iid[] = {};
	const SLboolean output_mix_req[] = {};
	//const SLInterfaceID ids[] = {SL_IID_VOLUME};
	//const SLboolean req[] = {SL_BOOLEAN_FALSE};

	result = (*osl_engine.audio_engine)->CreateOutputMix(osl_engine.audio_engine,&osl_engine.output_mix_object,0,output_mix_iid,output_mix_req);
	if(result != SL_RESULT_SUCCESS)
		LOGE("CreatOutputMix failed.");
	result = (*osl_engine.output_mix_object)->Realize(osl_engine.output_mix_object, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		LOGE("OutputMixObject realization failed.");

	//======================================================================================================


	//============================================= Setting up Audio Player ================================


	//==== define these ======
	int SBC_AUDIO_OUT_CHANNELS = 8;
	int SBC_AUDIO_OUT_BUFFER_SIZE = 256;
	int SBC_AUDIO_OUT_SAMPLE_RATE = 11025;
	// ===========================


	//Setting buffers as not in use
	for(int i = 0; i < SBC_AUDIO_OUT_CHANNELS; i++)
	{
		osl_engine.mSounds[i].mUsed = false;
	}

	//============================ Setting up data sources =================================

	SLDataLocator_AndroidSimpleBufferQueue loc_bufq;
	LOGE("1");
	loc_bufq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	loc_bufq.numBuffers = 2;
	LOGE("2");

	//jint speakers = 1;
	//jint channels = 2;
	//Format of data
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = 1;// mono audio
	format_pcm.samplesPerSec = SL_SAMPLINGRATE_11_025;
	format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	//Describing the input
	SLDataSource audio_source = {&loc_bufq, &format_pcm};
	audio_source.pLocator = &loc_bufq;
	audio_source.pFormat = &format_pcm;

	//Describing the output
	SLDataLocator_OutputMix data_locator_out;
	LOGE("3");
	data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	LOGE("4");
	data_locator_out.outputMix = osl_engine.output_mix_object;

	SLDataSink data_sink;
	data_sink.pLocator = &data_locator_out;
	data_sink.pFormat = NULL;
	//=================================================================================================

	//================================== Creating the Sound player ====================================
	const SLuint32 sound_player_iid_count = 3;
	const SLInterfaceID sound_player_iids[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean sound_player_reqs[] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


	result = (*osl_engine.audio_engine)->CreateAudioPlayer(osl_engine.audio_engine,&osl_engine.sound_player_object,&audio_source, &data_sink, sound_player_iid_count, sound_player_iids, sound_player_reqs);
	if(result != SL_RESULT_SUCCESS)
		LOGE("CreateAudioPlayer failed.");
	result = (*osl_engine.sound_player_object)->Realize(osl_engine.sound_player_object, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Audio player realization failed.");
	//Getting the three interfaces we requested above

	result = (*osl_engine.sound_player_object)->GetInterface(osl_engine.sound_player_object, SL_IID_PLAY, &osl_engine.sound_player);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Getting sound_player interface failed.");
	result = (*osl_engine.sound_player_object)->GetInterface(osl_engine.sound_player_object, SL_IID_BUFFERQUEUE, &osl_engine.sound_queue);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Getting soud_queue interface failed.");
	result = (*osl_engine.sound_player_object)->GetInterface(osl_engine.sound_player_object, SL_IID_VOLUME, &osl_engine.sound_volume);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Getting sound_volume interface failed.");
	//===========================================================================================================
	//=============== setting callback
	// called when current buffer is done playing to prepare the next buffer
	result = (*osl_engine.sound_queue)->RegisterCallback(osl_engine.sound_queue,test_sound_callback,(void*)&osl_engine);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Failed to register queue callback.");

	short mAudioSoundData1[SBC_AUDIO_OUT_BUFFER_SIZE];
	short mAudioSoundData2[SBC_AUDIO_OUT_BUFFER_SIZE];

	short* mActiveAudioSoundBuffer;
	//Filling both buffers with all 0s
	memset(mAudioSoundData1,0,sizeof(short) * SBC_AUDIO_OUT_BUFFER_SIZE);
	memset(mAudioSoundData2,0,sizeof(short) * SBC_AUDIO_OUT_BUFFER_SIZE);

	//Putting a short sine wave on the first buffer.
	for(int i = 0; i < SBC_AUDIO_OUT_BUFFER_SIZE; i++)
	{
		mAudioSoundData1[i] = (short) (15000 * sin(i*0.5));
	}
	//Point to the first buffer
	mActiveAudioSoundBuffer = mAudioSoundData1;

	//send two buffers

	//this is where buffer is wiped to all 0's, audio clip is placed in buffer, etc.
//	prepareSoundBuffer();
	result = (*osl_engine.sound_queue)->Enqueue(osl_engine.sound_queue, mActiveAudioSoundBuffer, sizeof(short) * SBC_AUDIO_OUT_BUFFER_SIZE);
//	swapSoundBuffers();
	//{
	if(mActiveAudioSoundBuffer == mAudioSoundData1)
		mActiveAudioSoundBuffer = mAudioSoundData2;
	else
		mActiveAudioSoundBuffer = mAudioSoundData2;
	//}

//	sendSoundBuffer();
//	sendSoundBuffer();


	result = (*osl_engine.sound_player)->SetPlayState(osl_engine.sound_player,SL_PLAYSTATE_PLAYING);
	if(result != SL_RESULT_SUCCESS)
		LOGE("Failed to set play state as SL_PLAYSTATE_PLAYING.");

	//run the engine loop
	while(1)
	{
		//Reading all pending events
		int ident;
		int events;
		struct android_poll_source *source;

		//When not animating: block waiting for events
		//When animating, handle all events then draw frame
		while((ident = ALooper_pollAll(engine.animating ? 0 : -1,NULL, &events,(void**)  &source))>=0)
		{
			//Process event
			if(source != NULL)
			{
				source->process(app,source);
			}
			//If sensor has data, process it
			if(ident == LOOPER_ID_USER)
			{
				if(engine.accelerometer_sensor != NULL)
				{
					ASensorEvent event;
					while(ASensorEventQueue_getEvents(engine.sensor_event_queue,&event,1) > 0)
					{
						//do nothing
						app_dummy();
						//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x,
						//	event.acceleration.y, event.acceleration.z);
					}
				}
			}

			//Check if exiting
			if(app->destroyRequested != 0)
			{
#ifdef DEBUG_MODE
				LOGI("LIFECYCLE: Exiting application.\n");
#endif
				engine.term_display();
				engine.term_data();
				delete &engine;
				return;
			}
		}
		if(engine.animating)
		{
			engine.state.angle += 0.01f;
			if(engine.state.angle > 1)
			{
				engine.state.angle = 0;
			}
			//Drawing throttled by screen update rate, no timing code needed here
			engine.draw_frame();
		}
	}
}
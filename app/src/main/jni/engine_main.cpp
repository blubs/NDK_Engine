#include "engine_main.h"


void android_main(struct android_app *app)
{
	//Making sure glue is not stripped
	app_dummy();

	Engine engine(app);
	//We never leave this scope anyways
	app->userData = &engine;


	//Trying to get files to load.
	File_Utils::asset_mgr = app->activity->assetManager;

	//Setting start time as now (a reference point)
	set_start_time();

	//====================================

#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: ANDROID_MAIN RAN\n");
#endif

	//Reads saved data and writes saved data
/*	const char* data = File_Utils::read_savedata("test.dat");
	LOGI("Read file: %s.\n",data);
	free((char*)data);
	File_Utils::write_savedata("test.dat");*/


	//	Reads a file containing 5 floating point values (0.0-4.0)
	/*const char* float_arr = File_Utils::load_raw_asset("test_float_array.skmf");
	LOGE("float arr: [%f,%f,%f,%f,%f]\n",((float*)float_arr)[0],((float*)float_arr)[1],((float*)float_arr)[2],((float*)float_arr)[3],((float*)float_arr)[4]);
	free((void*)float_arr);*/


	//Test audio file loading
	const Asset_File* test_sound_file = File_Utils::load_asset("raw_1000_10_5_sec_sweep_stereo.raw");
	engine.snd_ch.data = (char*)test_sound_file->data;
	//We want the length in sound samples
	engine.snd_ch.length = (test_sound_file->len)/sizeof(Stereo_Sample);
	engine.snd_ch.used = false;
	//Only needed the Asset_File to get the length
	free((void*)test_sound_file);

	engine.play_sound();

	float last_frame_time = 0.0;

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
			//Unused accelerometer update handling
			//If sensor has data, process it
			//if(ident == LOOPER_ID_USER)
			//{
			//	//if(engine.accelerometer_sensor != NULL)
			//	//{
			//		ASensorEvent event;
			//		while(ASensorEventQueue_getEvents(engine.sensor_event_queue,&event,1) > 0)
			//		{
			//			//do nothing
			//			app_dummy();
			//			//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x,
			//			//	event.acceleration.y, event.acceleration.z);
			//		}
			//	//}
			//}
			//Check if exiting
			if(app->destroyRequested != 0)
			{
#ifdef DEBUG_MODE
				LOGI("LIFECYCLE: Exiting application.\n");
#endif
				engine.term_data();
				return;
			}
		}

		if(engine.animating)
		{
			engine.draw_frame();

			//No guarantees that the we're actually drawing until gl_initialized is 1
			if(engine.gl_initialized)
			{
				//===== Temporary variable stuff =======
				engine.state.angle += 0.01f;
				if(engine.state.angle > 1)
				{
					engine.state.angle = 0;
				}
				//======================================

				//Drawing throttled by screen update rate, no timing code needed here
				static long frame = 0;
				//if(frame % 60 == 0)
				//	LOGE("60 frames passed\n");
				//LOGE("Frame: %ld, frame mod 60 = %ld\n",frame,(frame % 60));
				frame++;
				float ctime = time();

				engine.delta_time = ctime - last_frame_time;
				last_frame_time = ctime;
				//LOGE("delta_time = %f,  last_frame_time = %f, avg fps = %f\n",delta_time, last_frame_time,frame/time());
			}
		}
	}
}
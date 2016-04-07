#include "engine_main.h"

void android_main(struct android_app *app)
{
	//Making sure glue is not stripped
	app_dummy();

	//Engine* engine = new Engine(app);
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
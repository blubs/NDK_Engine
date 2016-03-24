#include "engine_main.h"
#include "Engine.h"

void android_main(struct android_app *app)
{
	//Making sure Glue is not stripped
	app_dummy();

	//Engine* engine = new Engine(app);
	Engine engine(app);
	//We never leave this scope anyways
	app->userData = &engine;

	//Assigning our assetManager
	//LOGE("============== LOOK HERE===================\n");
	//File_Utils::asset_mgr = app->activity->assetManager;
	//LOGE("============== LOOK HERE===================\n");

	//GLuint vertex_shader = GL_Utils::load_shader("shaders/test_vert.fsh",GL_VERTEX_SHADER);
	//LOGI("Vertex shader has a value of %d",vertex_shader);

	//GL_Utils::unload_shader(vertex_shader);

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
						LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x,
							event.acceleration.y, event.acceleration.z);
					}
				}
			}

			//Check if exiting
			if(app->destroyRequested != 0)
			{
				engine.term_display();
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
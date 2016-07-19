#include "engine_main.h"

#include "jni.h"


//these methods aren't being called, is the java vm even being started?
//=======================================================================
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGE("JNI OnLoad called");
	return 1;
}
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	LOGE("JNI OnUnload called");
}
//=======================================================================
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

	//============ Attempting to call java methods from cpp =============
	// First have to set up environment variables and make sure we can retrieve them
	LOGE("Starting Android Java Code");

	jvalue java_args[2];
	java_args[0].i = 2;
	java_args[1].i = 3;

	JNIEnv* env = app->activity->env;


	//Checking to make sure we have an env variable
	//LOGE("app: %p, activity: %p, env: %p\n",app,app->activity,app->activity->env);
	//env pointer is not null

	jclass java_cls;

	JavaVM* vm;
	//LOGE("vm before: %p",vm);
	//env->GetJavaVM(&vm);
	vm = app->activity->vm;

	JNIEnv* env_2;

	vm->GetEnv((void**)&env_2,JNI_VERSION_1_6);

	LOGE("Env from activity: %p, Env from VM: %p",env,env_2);

	//JNI_CreateJavaVM();


	//LOGE("vm after: %p",vm);
	LOGE("log class before: %d\n",(int)java_cls);
	//java_cls = env->FindClass("CLASS PROJECT NAME GOES HERE");
	LOGE("log class after: %d\n",(int)java_cls);
	//jclass global_java_cls_ref = (jclass) env->NewGlobalRef(java_cls);

	//jmethodID java_method = env->GetMethodID(global_java_cls_ref,"test","(II)Z");
	//TODO: somehow get java object instance reference
	//jboolean result = env->CallBooleanMethodA(object,java_method,java_args);

	//todo: call later to free global reference:
	//env->FreeGlobalRef(global_java_cls_ref);


	LOGE("Android Java Code done");
	//======================= end java test =============================

	//Reads saved data and writes saved data
/*	const char* data = File_Utils::read_savedata("test.dat");
	LOGI("Read file: %s.\n",data);
	free((char*)data);
	File_Utils::write_savedata("test.dat");*/

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
				engine.last_frame();
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
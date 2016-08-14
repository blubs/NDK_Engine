//
// Created by F1 on 3/23/2016.
//

#include "Engine.hpp"


Engine::Engine (struct android_app *droid_app)
{
	app = droid_app;

	app->userData = this;
	app->onAppCmd = Engine::handle_cmd;
	app->onInputEvent = Engine::handle_input;

	//Setting up accelerometer stuff
	sensor_manager = ASensorManager_getInstance();
	//Unused accelerometer sensor reference
	//accelerometer_sensor = ASensorManager_getDefaultSensor(sensor_manager,ASENSOR_TYPE_ACCELEROMETER);
	sensor_event_queue = ASensorManager_createEventQueue(sensor_manager, droid_app->looper, LOOPER_ID_USER, NULL, NULL);

	//Restore from a saved state
	if(droid_app->savedState != NULL)
	{
		//Really not sure when this is running, I'll add debug code so I can be informed when
		LOGE("NOT AN ERROR: notifying that previous saved state was not null, set state to prev state.\n");
		state = *(struct saved_state *) droid_app->savedState;
	}

	audio_engine = new Audio_Engine();

	jnii = new JNI_Interface(app->activity);

	game = new Game();
}

Engine::~Engine()
{
	if(audio_engine)
		delete audio_engine;
	if(jnii)
		delete jnii;
	if(game)
		delete game;

	term_data();
}


void Engine::handle_cmd (struct android_app *app, int32_t cmd)
{
	Engine *eng = (Engine *) app->userData;

	switch(cmd)
	{
		//Saving our current state
		case APP_CMD_SAVE_STATE:
			eng->app->savedState = malloc(sizeof(struct saved_state));
			*((struct saved_state *) eng->app->savedState) = (eng->state);

			eng->app->savedStateSize = sizeof(struct saved_state);
			break;
			//Window is being shown
		case APP_CMD_INIT_WINDOW:
			if(eng->app->window != NULL)
			{
				if(!eng->data_initialized)
				{
					LOGE("attempted to draw screen before loading data.\n");
					if(!eng->init_data())
					{
						return;
					}
				}
				if(eng->init_display() == -1)
				{
					LOGE("Init display returns -1");
					return;
				}
				eng->draw_frame();
			}
			break;
			//Window is being hidden
		case APP_CMD_TERM_WINDOW:
			eng->term_display();
			break;
			//App gained focus, start monitoring accelerometer
		case APP_CMD_GAINED_FOCUS:
			//if(eng->accelerometer_sensor != NULL)
			//{
			//	ASensorEventQueue_enableSensor(eng->sensor_event_queue,eng->accelerometer_sensor);
			//60 samples per second
			//	ASensorEventQueue_setEventRate(eng->sensor_event_queue,eng->accelerometer_sensor, (1000L/60)*1000);
			//}
			eng->audio_engine->start_audio();
			break;
			//App lost focus, stop monitoring acceleromter, and stop animating
		case APP_CMD_LOST_FOCUS:
			//if(eng->accelerometer_sensor != NULL)
			//{
			//	ASensorEventQueue_disableSensor(eng->sensor_event_queue,eng->accelerometer_sensor);
			//}
			eng->audio_engine->pause_audio();
			eng->animating = 0;
			eng->draw_frame();
			break;
		default:
			break;
	}
	return;
}

int32_t Engine::handle_input (struct android_app *app, AInputEvent *event)
{
	Engine *eng = (Engine *) app->userData;
	int type = AInputEvent_getType(event);

	if(type == AINPUT_EVENT_TYPE_MOTION)
	{
		eng->animating = 1;
		eng->state.x = AMotionEvent_getX(event, 0) / eng->width;
		eng->state.y = AMotionEvent_getY(event, 0) / eng->height;

		int32_t motion_action = AMotionEvent_getAction(event);
		int motion_type = motion_action & AMOTION_EVENT_ACTION_MASK;

		int event_type = 0;

		switch(motion_type)
		{
			case AMOTION_EVENT_ACTION_DOWN:
				event_type = Game::INPUT_EVENT_ON_TOUCH_DOWN;
				break;
			case AMOTION_EVENT_ACTION_UP:
				event_type = Game::INPUT_EVENT_ON_TOUCH_RELEASE;
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				event_type = Game::INPUT_EVENT_ON_TOUCH_MOVE;
				break;
			default:
				break;
		}
		if(eng->game && event_type)
		{
			eng->game->handle_input(eng->state.x,eng->state.y,event_type);
		}
		return 1;
	}
	return 0;
}

int Engine::init_display ()
{
#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: init display started.\n");
#endif
	//Setting up OPENGLES
	const EGLint config_attribs[] = {
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_BLUE_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_RED_SIZE, 8,
	EGL_DEPTH_SIZE, 24,
	EGL_NONE
	};
	EGLint w, h, num_configs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	//Should we check for this?
	/*if(display == NULL)
	{
		LOGW("eglGetDisplay returned NULL");
		return -1;
	}*/
	eglInitialize(display, 0, 0);

	//Choosing first config that matches our criteria)
	eglChooseConfig(display, config_attribs, &config, 1, &num_configs);
	surface = eglCreateWindowSurface(display, config, app->window, NULL);

	const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

	context = eglCreateContext(display, config, NULL, context_attribs);

	if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		//error, should return -1 or so
		LOGW("eglMakeCurrent Failed");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	egl_display = display;
	egl_context = context;
	egl_surface = surface;
	width = w;
	height = h;
	//state->angle = 0;
	state.angle = 0;

	animating = 1;

	//====== GL Initialization code ======
	if(!init_gl())
		return -1;

	//Resume audio if it was paused previously
	if(audio_engine->sl_audio_player_interface != NULL)
		audio_engine->start_audio();

#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: init display finished.\n");
#endif

	return 0;
}

void Engine::term_display ()
{
#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: DISPLAY TERMINATED\n");
#endif

	//Need to run code that stops the executing code
	audio_engine->pause_audio();
	//====== GL termination code ======
	term_gl();
	//=================================
	if(egl_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if(egl_context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(egl_display, egl_context);
		}
		if(egl_surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(egl_display, egl_surface);
		}
		eglTerminate(egl_display);
	}
	animating = 0;
	egl_display = EGL_NO_DISPLAY;
	egl_context = EGL_NO_CONTEXT;
	egl_surface = EGL_NO_SURFACE;

}

int Engine::init_gl ()
{
	LOGI("Init gl");
	//Init gl state
	//At this stage, all of the shaders have already been loaded.
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);


	//enable blend function
	glEnable(GL_BLEND);
	//set blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	game->init_gl();

	glViewport(0, 0, width, height);
	//glDepthRangef(0.0f,1.0f); useless line
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	LOGI("Init gl finished");

	gl_initialized = true;
	return 1;
}

void Engine::term_gl ()
{
	game->term_gl();
	gl_initialized = false;
}

int Engine::init_data ()
{
	LOGI("init_data...\n");
	if(!game->load_assets())
		return 0;
	data_initialized = true;
	return 1;
}

void Engine::term_data ()
{
	game->unload_assets();
	data_initialized = false;
}

void Engine::first_frame()
{
	game->screen_width = width;
	game->screen_height = height;
	game->jnii = jnii;
	game->start();
	Audio_Engine::set_audio_listener(game->audio_listener);
}

//This is where we terminate any memory that was alllocated mid-game
void Engine::last_frame()
{
	game->finish();
}

void Engine::draw_frame ()
{
	//Need to initialize data before the screen context has been created.
	if(!data_initialized)
	{
		if(!init_data())
			return;
	}
#ifdef DEBUG_MODE
	static bool was_rendering = true;
	if(!egl_display)
	{
		if(was_rendering)
		{
			LOGI("LIFECYCLE: RENDER ABORTED, display not initialized\n");
			was_rendering = false;
		}
		return;
	}
	if(!was_rendering)
	{
		LOGI("LIFECYCLE: rendering...\n");
		was_rendering = true;
	}
#endif

	if(!egl_display)
	{
		return;
	}

	static bool is_first_frame = true;
	if(is_first_frame)
	{
		first_frame();
		is_first_frame = false;
	}

	game->update();
	game->render();
	eglSwapBuffers(egl_display, egl_surface);
}

float Engine::delta_time = 0.0f;
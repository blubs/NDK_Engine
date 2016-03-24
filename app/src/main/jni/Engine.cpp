//
// Created by F1 on 3/23/2016.
//

#include "Engine.h"

Engine::Engine(struct android_app* mApp)
{
	app = mApp;

	app->userData = this;
	app->onAppCmd = Engine::handle_cmd;
	app->onInputEvent = Engine::handle_input;

	//Setting up accelerometer stuff
	sensor_manager = ASensorManager_getInstance();
	accelerometer_sensor = ASensorManager_getDefaultSensor(sensor_manager,ASENSOR_TYPE_ACCELEROMETER);
	sensor_event_queue = ASensorManager_createEventQueue(sensor_manager, mApp->looper, LOOPER_ID_USER,NULL,NULL);

	//Restore from a saved state
	if(mApp->savedState != NULL)
	{
		//*state = *(struct saved_state*) mApp->savedState;
		state = *(struct saved_state*) mApp->savedState;
	}
}

void Engine::handle_cmd(struct android_app *app, int32_t cmd)
{
	Engine* eng = (Engine*)app->userData;

	switch(cmd)
	{
		//Saving our current state
		case APP_CMD_SAVE_STATE:
			eng->app->savedState = malloc(sizeof(struct saved_state));
			//*((struct saved_state*) eng->app->savedState) = *(eng->state);
			*((struct saved_state*) eng->app->savedState) = (eng->state);

			eng->app->savedStateSize = sizeof(struct saved_state);
			break;
			//Window is being shown
		case APP_CMD_INIT_WINDOW:
			if(eng->app->window != NULL)
			{
				eng->init_display();
				eng->draw_frame();
			}
			break;
			//Window is being hidden
		case APP_CMD_TERM_WINDOW:
			eng->term_display();
			break;
			//App gained focus, start monitoring accelerometer
		case APP_CMD_GAINED_FOCUS:
			if(eng->accelerometer_sensor != NULL)
			{
				ASensorEventQueue_enableSensor(eng->sensor_event_queue,eng->accelerometer_sensor);
				//60 samples per second
				ASensorEventQueue_setEventRate(eng->sensor_event_queue,eng->accelerometer_sensor, (1000L/60)*1000);
			}
			break;
			//App lost focus, stop monitoring acceleromter, and stop animating
		case APP_CMD_LOST_FOCUS:
			if(eng->accelerometer_sensor != NULL)
			{
				ASensorEventQueue_disableSensor(eng->sensor_event_queue,eng->accelerometer_sensor);
			}
			eng->animating = 0;
			eng->draw_frame();
			break;
		default:
			break;
	}
	return;
}
int32_t Engine::handle_input(struct android_app *app, AInputEvent *event)
{
	Engine* eng = (Engine*)app->userData;

	if(AInputEvent_getType(event)== AINPUT_EVENT_TYPE_MOTION)
	{
		eng->animating = 1;
		//eng->state->x = (int32_t)AMotionEvent_getX(event,0);
		//eng->state->y = (int32_t)AMotionEvent_getY(event,0);
		eng->state.x = (int32_t)AMotionEvent_getX(event,0);
		eng->state.y = (int32_t)AMotionEvent_getY(event,0);
		return 1;
	}
	return 0;
}

int Engine::init_display()
{
	//Setting up OPENGLES
	const EGLint attribs[] = {
		   EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		   EGL_BLUE_SIZE, 8,
		   EGL_GREEN_SIZE, 8,
		   EGL_RED_SIZE, 8,
		   EGL_NONE
	};
	EGLint w,h,numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display,0,0);

	//Choosing first config that matches our criteria
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);
	surface = eglCreateWindowSurface(display, config, app->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		//error, should return -1 or so
		LOGW("eglMakeCurrent Failed");
		return -1;
	}

	eglQuerySurface(display,surface,EGL_WIDTH, &w);
	eglQuerySurface(display,surface,EGL_HEIGHT, &h);

	gl_display = display;
	gl_context = context;
	gl_surface = surface;
	width = w;
	height = h;
	//state->angle = 0;
	state.angle = 0;

	//Init gl state
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


	animating = 1;
	return 0;
}

void Engine::term_display()
{
	if(gl_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(gl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if(gl_context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(gl_display, gl_context);
		}
		if(gl_surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(gl_display, gl_surface);
		}
		eglTerminate(gl_display);
	}
	animating = 0;
	gl_display = EGL_NO_DISPLAY;
	gl_context = EGL_NO_CONTEXT;
	gl_surface = EGL_NO_SURFACE;
}

void Engine::draw_frame()
{
	if(gl_display == NULL)
	{
		return;
	}

	//Filling the screen with a color
	glClearColor(((float)state.x)/width, 0.0f/*state.angle*/, ((float)state.y)/height,1);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl_display, gl_surface);
}

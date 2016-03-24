//
// Created by F1 on 3/23/2016.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "common.h"

// Saved State Data
struct saved_state
{
	float angle;
	int32_t x;
	int32_t y;
};


class Engine
{
public:
	//remove this
	Engine() {};
	Engine(struct android_app *app);
	~Engine() {};

	static void handle_cmd(struct android_app *app, int32_t cmd);
	static int32_t handle_input(struct android_app *app, AInputEvent *event);

	void term_display();
	int init_display();

	void draw_frame();

	EGLDisplay gl_display;
	EGLSurface gl_surface;
	EGLContext gl_context;

	struct android_app *app;

	ASensorManager *sensor_manager;
	const ASensor *accelerometer_sensor;
	ASensorEventQueue *sensor_event_queue;

	int32_t width;
	int32_t height;
	struct saved_state state;
	//struct saved_state* state;
	int animating;
};


#endif //ENGINE_ENGINE_H

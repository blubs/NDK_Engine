//
// Created by F1 on 3/23/2016.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "common.h"

// Saved State Data
struct saved_state
{
	float angle = 0.0f;
	int32_t x = 0;
	int32_t y = 0;
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

	int load_assets();
	void unload_assets();

	int load_shaders();
	void unload_shaders();

	int init_gl();
	void term_gl();
	//bool gl_initialized = false;


	int init_data();
	void term_data();
	bool data_initialized = false;


	GLuint test_frag_shader = 0;
	const char* frag_shader_src = 0;
	const char* frag_shader_name = 0;
	GLuint test_vert_shader = 0;
	const char* vert_shader_src = 0;
	const char* vert_shader_name = 0;
		GLint shader_vert_pos_loc = -1;
		GLint shader_fill_color_loc = -1;
		GLint shader_tex_loc = -1;
		GLint shader_uv_loc = -1;
	GLuint gl_program = 0;
	GLuint test_tex = 0;
	GLuint texture_id = 0;

	void draw_frame();

	EGLDisplay egl_display = 0;
	EGLSurface egl_surface = 0;
	EGLContext egl_context = 0;

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

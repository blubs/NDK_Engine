//
// Created by F1 on 3/23/2016.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "common.h"
#include "Misc.h"
#include "SL_Utils.h"
#include "File_Utils.h"
#include "GL_Utils.h"
#include "Camera.h"
#include "Player.h"



// Saved State Data
struct saved_state
{
	float angle = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
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

	int init_sl();
	void term_sl();
	//OSL Sound Engine Data
	SLObjectItf sl_engine = NULL;
	SLEngineItf sl_engine_interface = NULL;
	SLObjectItf sl_output_mix = NULL;
	SLObjectItf sl_audio_player = NULL;
	SLPlayItf sl_audio_player_interface = NULL;
	SLBufferQueueItf sl_buffer_queue_interface = NULL;
	SLVolumeItf sl_volume_interface = NULL;
	//void sl_buffer_callback(SLBufferQueueItf snd_queue, void* context);
	void start_audio();
	void stop_audio();
	void pause_audio();

	//Engine Sound Data
	//TODO: make sounds played be handled per entity
	Stereo_Sample audio_buffer1[SND_AUDIO_BUFFER_SIZE];
	Stereo_Sample audio_buffer2[SND_AUDIO_BUFFER_SIZE];
	Stereo_Sample* active_audio_buffer;
	Stereo_Sample* inactive_audio_buffer;
	//Sound playing_sound[8];
	//Test methods for testing audio file playing
	Sound_Channel snd_ch;
	void play_sound();



	int load_shaders();
	void unload_shaders();

	int init_gl();
	void term_gl();
	//bool gl_initialized = false;


	int init_data();
	void term_data();
	bool data_initialized = false;


	const char* frag_shader_src = 0;
	const char* frag_shader_name = 0;

	const char* vert_shader_src = 0;
	const char* vert_shader_name = 0;

	//FIXME remove this after shader.h is done
/*
	GLuint test_frag_shader = 0;
	GLuint test_vert_shader = 0;
		GLint shader_vert_pos_loc = -1;
		GLint shader_fill_color_loc = -1;
		GLint shader_tex_loc = -1;
		GLint shader_uv_loc = -1;
		GLint shader_mvp_loc = -1;
	GLuint gl_program = 0;
	GLuint test_tex = 0;
	GLuint texture_id = 0;
*/
	const char* test_texture = 0;
	void draw_frame();

	EGLDisplay egl_display = 0;
	EGLSurface egl_surface = 0;
	EGLContext egl_context = 0;

	struct android_app *app;

	ASensorManager *sensor_manager;
	//Unused accelerometer reference
	//const ASensor *accelerometer_sensor;
	ASensorEventQueue *sensor_event_queue;

	int32_t width;
	int32_t height;
	struct saved_state state;
	//struct saved_state* state;
	int animating;

	Camera* camera;
};


#endif //ENGINE_ENGINE_H

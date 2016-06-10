//
// Created by F1 on 3/23/2016.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "common.h"
#include "misc.h"
#include "SL_Utils.h"
#include "File_Utils.h"
#include "GL_Utils.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "Material.h"
#include "Skel_Model.h"
#include "Skeleton.h"
#include "Texture.h"
#include "UI_Text.h"


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
	void term();
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
	bool gl_initialized = false;


	int init_data();
	void term_data();
	bool data_initialized = false;



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
*/
	Shader* test_shader = NULL;
	Shader* test_skeletal_shader = NULL;

	Material* mat_red = NULL;
	Material* mat_blue = NULL;

	Shader* mesh_shader = NULL;
	Material* mesh_mat = NULL;

	Material* skeletal_mat = NULL;

	Shader* text_shader = NULL;
	Material* text_mat = NULL;


	//This is independent from shader
	Texture* test_texture = NULL;

	Skel_Model* test_arms = NULL;
	Skeleton* player_skel = NULL;


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

	Camera* camera = NULL;
	Player* player = NULL;
	Entity_Bone_Joint* cam_to_bone = NULL;

	UI_Text* test_text = NULL;

	static float delta_time;
};


#endif //ENGINE_ENGINE_H

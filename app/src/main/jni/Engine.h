//
// Created by F1 on 3/23/2016.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "common.h"
#include "misc.h"
#include "Audio_Engine.h"
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
#include "UI_Image.h"
#include "Static_Model.h"
#include "Cube_Map.h"


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
	~Engine();

	static void handle_cmd(struct android_app *app, int32_t cmd);
	static int32_t handle_input(struct android_app *app, AInputEvent *event);

	void term_display();
	int init_display();

	int load_assets();
	void unload_assets();

	Audio_Engine* audio_engine;

	int load_shaders();
	void unload_shaders();

	int init_gl();
	void term_gl();
	bool gl_initialized = false;


	int init_data();
	void term_data();
	bool data_initialized = false;

	Sound_Sample* test_pulse = NULL;

	Shader* test_shader = NULL;

	Material* mat_red = NULL;
	Material* mat_blue = NULL;

	Shader* skel_color_shader = NULL;
	Material* skel_color_mat= NULL;

	Shader* static_color_shader = NULL;
	Material* static_color_mat = NULL;

	Shader* text_shader = NULL;
	Material* text_mat = NULL;


	//This is independent from shader
	Texture* test_texture = NULL;
	Texture* char_set = NULL;

	Skel_Model* test_arms = NULL;
	Skeleton* player_skel = NULL;

	Static_Model* model_prim_cube = NULL;
	Static_Model* model_prim_quad = NULL;


	void draw_frame();

	void first_frame();
	void last_frame();

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

	Entity* test_sound_source = NULL;

	UI_Text* test_text = NULL;
	UI_Image* test_img = NULL;

	static float delta_time;
};


#endif //ENGINE_ENGINE_H

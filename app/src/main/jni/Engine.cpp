//
// Created by F1 on 3/23/2016.
//

#include "Engine.h"


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

	//===================== Setting up game world objects =======================

	//========================== Instantiating shaders ==========================
	test_shader = (Shader *) malloc(sizeof(Shader));
	skel_color_shader = (Shader*) malloc(sizeof(Shader));
	static_color_shader = (Shader*) malloc(sizeof(Shader));
	text_shader = (Shader*) malloc(sizeof(Shader));

	//========================= Instantiating materials =========================
	mat_red = new Material();
	mat_blue = new Material();
	skel_color_mat = new Material();
	static_color_mat = new Material();
	text_mat = new Material();

	//===================== Creating Skeletal Models ============================
	test_arms = (Skel_Model*) malloc(sizeof(Skel_Model));

	//================== Instantiating Static Models ============================
	model_prim_cube = (Static_Model*) malloc(sizeof(Static_Model));
	model_prim_quad = (Static_Model*) malloc(sizeof(Static_Model));

	//======================= Instantiating Textures ==========================
	test_texture = (Texture*) malloc(sizeof(Texture));
	char_set = (Texture*) malloc(sizeof(Texture));

	//======================= Instantiating UI Objects ========================
	test_text = (UI_Text*) malloc(sizeof(UI_Text));
	test_img = (UI_Image*) malloc(sizeof(UI_Image));

	//====================== Instantiating Objects ============================
	player_skel = new Skeleton();
	camera = new Camera();
	player = new Player();
	cam_to_bone = new Entity_Bone_Joint();
	test_sound_source = new Entity();

	skybox = new Skybox();
	test_cube_map = (Cube_Map*) malloc(sizeof(Cube_Map));

	//=================== Setting up temp sound containers ====================
	test_pulse = (Sound_Sample*) malloc(sizeof(Sound_Sample));

	//===========================================================================
}

Engine::~Engine()
{
	if(audio_engine)
		delete audio_engine;

	term_data();

	if(test_pulse)
		free(test_pulse);

	//Deleting Materials
	if(mat_red)
		delete mat_red;
	if(mat_blue)
		delete mat_blue;
	if(skel_color_mat)
		delete skel_color_mat;
	if(static_color_mat)
		delete static_color_mat;
	if(text_mat)
		delete text_mat;


	//Freeing shaders
	if(test_shader)
		free(test_shader);
	if(skel_color_shader)
		free(skel_color_shader);
	if(static_color_shader)
		free(static_color_shader);
	if(text_shader)
		free(text_shader);

	//Freeing models
	if(test_arms)
		free(test_arms);
	if(model_prim_cube)
		free(model_prim_cube);
	if(model_prim_quad)
		free(model_prim_quad);

	//Freeing textures
	if(test_texture)
		free(test_texture);
	if(char_set)
		free(char_set);
	if(test_text)
		free(test_text);
	if(test_img)
		free(test_img);

	//Deleting other objects
	if(player_skel)
		delete player_skel;
	if(player)
		delete player;
	if(test_sound_source)
		delete test_sound_source;
	if(camera)
		delete camera;
	if(cam_to_bone)
		delete cam_to_bone;

	if(test_cube_map)
		free(test_cube_map);
	if(skybox)
		delete skybox;
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
		/*int32_t motion_action = AMotionEvent_getAction(event);
		int motion_type = motion_action & AMOTION_EVENT_ACTION_MASK;
		float x = 0;
		float y = 0;
		switch(motion_type)
		{
			case AMOTION_EVENT_ACTION_DOWN:
				x = AMotionEvent_getX(event, 0) / eng->width;
				y = AMotionEvent_getY(event, 0) / eng->height;//0 being pointer index
				//LOGI("Amotion event action down: (%.4f,%.4f)\n",x,y);
				break;
			case AMOTION_EVENT_ACTION_UP:
				x = AMotionEvent_getX(event, 0) / eng->width;
				y = AMotionEvent_getY(event, 0) / eng->height;//0 being pointer index
				//LOGI("Amotion event action up: (%.4f,%.4f)\n",x,y);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				x = AMotionEvent_getX(event, 0) / eng->width;
				y = AMotionEvent_getY(event, 0) / eng->height;//0 being pointer index
				//LOGI("Amotion event action move: (%.4f,%.4f)\n",x,y);
				break;
			default:
				break;
		}*/

		eng->animating = 1;
		eng->state.x = AMotionEvent_getX(event, 0) / eng->width;
		eng->state.y = AMotionEvent_getY(event, 0) / eng->height;
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

//========================================= Loading assets ======================================

int Engine::load_shaders ()
{
	test_shader->load("minimal.vert","minimal.frag");
	skel_color_shader->load("skeletal_color.vert","skeletal_color.frag");
	static_color_shader->load("static_color.vert","static_color.frag");
	text_shader->load("monochrome_transparent.vert","monochrome_transparent.frag");
	skybox->load_shader();
	return 1;
}


//Returns 0 on fail, returns 1 on success.
int Engine::load_assets ()
{
	//Loading the test texture.
	test_texture->load("tex.pkm",512,512);
	char_set->load("char_set.pkm",2048,2048);

	test_cube_map->load("cube_maps/test_cube_map.pkm",512);

	test_arms->load_model("test_arms.skmf");
	model_prim_cube->load_model("primitive_cube.stmf");
	model_prim_quad->load_model("primitive_quad.stmf");

	player_skel->load("player_skeleton.sksf");
	player_skel->load_animation("player_animations/run.skaf");
	player_skel->load_animation("player_animations/showcase_hands.skaf");
	player_skel->load_animation("player_animations/speed_vault.skaf");

	//Sounds
	test_pulse->load("test_audio_pulse.raw");
	return 1;
}

//=================================================================================================

//========================================= Unloading assets ======================================
void Engine::unload_shaders ()
{
	test_shader->unload();
	skel_color_shader->unload();
	static_color_shader->unload();
	text_shader->unload();
	skybox->unload_shader();
}

void Engine::unload_assets ()
{
	//Sounds
	if(test_pulse)
		test_pulse->unload();
	//Textures
	if(test_texture)
		test_texture->unload();
	if(char_set)
		char_set->unload();
	if(test_cube_map)
		test_cube_map->unload();
	//Models
	if(test_arms)
		test_arms->unload_model();
	if(model_prim_cube)
		model_prim_cube->unload_model();
	if(model_prim_quad)
		model_prim_quad->unload_model();
	//Skeletons
	if(player_skel)
		player_skel->unload();
}
//=================================================================================================

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


	//Initializing shader
	//Populating parameter arrays

	GLuint param_types[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_VERT_COLORS,
		Shader::PARAM_VERT_UV1,
		Shader::PARAM_TEXTURE_DIFFUSE,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_TEST_FIELD
	};
	const char *param_names[] =
	{
		"vert_pos",
		"fill_color",
		"src_tex_coord",
		"tex",
		"mvp",
		"test_color_param"
	};
	uint param_count = 6;

	test_shader->init_gl(param_types, param_names,param_count);

	//======================================= Initializing the UI text shader =================================
	GLuint text_param_types[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_VERT_UV1,
		Shader::PARAM_TEXTURE_DIFFUSE,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_COLOR_MULT,
		Shader::PARAM_COLOR_ADD
	};
	const char *text_param_names[] =
	{
		"vert_pos",
		"src_tex_coord",
		"tex",
		"mvp",
		"mult_color",
		"add_color"
	};
	uint text_param_count = 6;

	text_shader->init_gl(text_param_types, text_param_names,text_param_count);

	//========================================= Initializing the skeletal mesh shader ================================

	GLuint skel_mesh_params[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_VERT_UV1,
		Shader::PARAM_VERT_NORMALS,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_M_IT_MATRIX,
		Shader::PARAM_BONE_INDICES,
		Shader::PARAM_BONE_WEIGHTS,
		Shader::PARAM_BONE_MATRICES,
		Shader::PARAM_BONE_IT_MATRICES
	};
	const char *skel_mesh_param_names[] =
	{
		"vert_pos",
		"vert_uv",
		"vert_nor",
		"mvp",
		"m_IT",
		"bone_index",
		"bone_weight",
		"bone",
		"bone_IT"
	};
	skel_color_shader->init_gl(skel_mesh_params, skel_mesh_param_names, 9);

	//=========================================== Initializing Static Mesh Color Shader =====================

	GLuint static_mesh_params[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_VERT_UV1,
		Shader::PARAM_VERT_UV2,
		Shader::PARAM_VERT_NORMALS,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_M_IT_MATRIX,
		Shader::PARAM_COLOR_MULT
	};
	const char *static_mesh_param_names[] =
	{
		"vert_pos",
		"vert_uv_1",
		"vert_uv_2",
		"vert_nor",
		"mvp",
		"m_IT",
		"color"
	};
	static_color_shader->init_gl(static_mesh_params, static_mesh_param_names, 7);

	//==================================== Loading textures =======================================
	test_texture->init_gl();
	char_set->init_gl();
	test_cube_map->init_gl();

	//==================================== Setting up Mesh VBOs ====================================
	test_arms->init_gl();
	model_prim_cube->init_gl();
	model_prim_quad->init_gl();

	skybox->init_gl();
	//========================================================================
	glViewport(0, 0, width, height);
	//glDepthRangef(0.0f,1.0f); useless line
	glClearColor(1, 1, 1, 1);
	LOGI("Init gl finished");

	gl_initialized = true;
	return 1;
}

void Engine::term_gl ()
{
	//Terminating all loaded shaders
	test_shader->term_gl();
	skel_color_shader->term_gl();
	static_color_shader->term_gl();
	text_shader->term_gl();


	//Terminating all loaded models
	test_arms->term_gl();
	model_prim_cube->term_gl();
	model_prim_quad->term_gl();

	skybox->term_gl();

	test_texture->term_gl();
	char_set->term_gl();
	test_cube_map->term_gl();

	gl_initialized = false;
}

int Engine::init_data ()
{
	LOGI("init_data...\n");
	if(!load_shaders())
		return 0;
	if(!load_assets())
		return 0;

	data_initialized = true;
	return 1;
}

void Engine::term_data ()
{
	unload_shaders();
	unload_assets();
	data_initialized = false;
}

void Engine::first_frame()
{
	player_skel->set_default_anim(0,Skeleton::END_TYPE_LOOP);
	player->mat = skel_color_mat;
	test_arms->skel = player_skel;

	//============================= Setting up materials ================================

	mat_red->set_shader(test_shader);
	mat_blue->set_shader(test_shader);
	text_mat->set_shader(text_shader);
	skel_color_mat->set_shader(skel_color_shader);
	static_color_mat->set_shader(static_color_shader);


	//Setting up fixed shader parameters
	float temp_color[] = {1.0f, 1.0f, 0.0f, 1.0f};
	static_color_mat->set_fixed_shader_param(Shader::PARAM_COLOR_MULT,temp_color,sizeof(float)*4);
	float temp_color_red[] = {1.0f, 0.4f, 0.4f, 1.0f};
	mat_red->set_fixed_shader_param(Shader::PARAM_TEST_FIELD, temp_color_red, sizeof(float) * 4);
	float temp_color_blue[] = {0.4f, 0.4f, 1.0f, 1.0f};
	mat_blue->set_fixed_shader_param(Shader::PARAM_TEST_FIELD, temp_color_blue, sizeof(float) * 4);



	//===================================================================================

	skybox->set_cube_map(test_cube_map);

	test_sound_source->model = model_prim_cube;
	test_sound_source->mat = static_color_mat;

	test_text->init(text_mat,char_set);
	//test_text->set_text("test\nT  E\n\nST !@\n#$%^&*()");
	test_text->set_text("Pause\npause\nPAUSE\n\nPlay\nplay\nPLAY\n\nExit\nexit\nEXIT\n\ntest_text->set_text(\"Stuff\")");

	//Place in top leftish corner
	test_text->pos.x = -width * 0.4f;
	test_text->pos.y = height * 0.4f;

	test_img->init(text_mat,test_texture);
	test_img->pos.x = width*0.5f - 100.0f;
	test_img->pos.y = height*0.5f - 100.0f;
	test_img->scale.x = 200.0f;
	test_img->uv_maxs.x = 0.6f;
	test_img->uv_mins.y = 0.5f;
	test_img->maintain_aspect_ratio = true;

	//test_img->scale.x = 0.2f;

	player->player_model = test_arms;
	player->skel = player_skel;


	camera->parent = cam_to_bone;
	cam_to_bone->parent_skel = player_skel;
	cam_to_bone->parent_bone_index = 8; //head bone is at index 8, we could add methods for finding the bone
	// but we don't need all of that at the moment (since camera is never going to be parented to anything but that bone)
	player_skel->parent = player;
	camera->set_persp_view(90.0f * DEG_TO_RAD, width,height, 0.01f, 1000.0f);
	camera->set_ortho_view(width,height,0.0001f,1.0f);

	Audio_Engine::set_audio_listener(camera);
}

//This is where we terminate any memory that was alllocated mid-game
void Engine::last_frame()
{
	test_text->term();
	test_img->term();
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	//Filling the screen with a color
	//glClearColor(state.x, 0.0f/*state.angle*/, state.y, 1);

	//Setting all transforms to be recalculated
	player_skel->transform_calculated = false;
	camera->transform_calculated = false;
	player->transform_calculated = false;
	test_sound_source->transform_calculated = false;
	cam_to_bone->transform_calculated = false;


	//glClear(GL_COLOR_BUFFER_BIT);

	//================================== Begin Drawing test boxes ==========================================

	const float cube_vertices[] =
	{
		//Front quad
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		//Back quad
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		//Right quad
		//Left quad
		//Top quad
		//Bottom quad
	};
	const float cube_uvs[] =
	{
		//Front quad
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//Back quad
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//Right quad
		//Left quad
		//Top quad
		//Bottom quad
	};
	const float cube_colors[] =
	{
		//Front quad
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		//Back quad
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		//Right quad
		//Left quad
		//Top quad
		//Bottom quad
	};

	camera->pos = Vec3::ZERO();
	camera->angles = Vec3::ZERO();
	//Pitch
	//camera->angles.x = ((0.5f - state.y) * TWO_PI);
	//Yaw
	//camera->angles.y = (0.5f - state.x) * TWO_PI;
	//Roll
	//camera->angles.z = 0.0f;
	camera->update_view_matrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Referencing vertices by index
	unsigned int cube_tris[] =
	{
		//Front quad
		0, 1, 2, 2, 3, 0,
		//Back quad
		4, 5, 6, 6, 7, 4,
		//Right quad
		1, 4, 7, 7, 2, 1,
		//Left quad
		0, 3, 6, 6, 5, 0,
		//Top quad
		4, 1, 0, 0, 5, 4,
		//Bottom quad
		6, 3, 2, 2, 7, 6,
	};

	//TODO: material should hold these buffers
	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);
	//Bind the buffer to set the data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int)/*size of indices*/, cube_tris, GL_STATIC_DRAW);

	//Getting the texture gl_id
	GLuint tex_gl_id = test_texture->gl_id;

	int i = 0;
	int j = 0;
	int k = 0;
	//Drawing a 3d array of cubes
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			for(k = 0; k < 4; k++)
			{
				if(i == 2 || j == 2 || k == 2)
					continue;
				Vec3 pos(5.0f * (i - 2.0f), 5.0f * (j - 2.0f), 5.0f * (k - 2.0f));

				Mat4 model_pos = Mat4::TRANSLATE(pos);
				//Mat4 model_transform = model_pos * model_rot; //don't rotate
				Mat4 model_transform = model_pos; //don't rotate
				Mat4 mvp = camera->persp_proj_m * camera->view_m * model_transform;

				//Make a checker board pattern of cubes (alternating materials between each subsequent cube)
				if((i + j + k) % 2)
				{
					mat_red->bind_material();
					mat_red->bind_value(Shader::PARAM_VERTICES, (void *) cube_vertices);
					mat_red->bind_value(Shader::PARAM_VERT_UV1, (void *) cube_uvs);
					mat_red->bind_value(Shader::PARAM_VERT_COLORS, (void *) cube_colors);
					mat_red->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) tex_gl_id);
					mat_red->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);
				}
				else
				{
					mat_blue->bind_material();
					mat_blue->bind_value(Shader::PARAM_VERTICES, (void *) cube_vertices);
					mat_blue->bind_value(Shader::PARAM_VERT_UV1, (void *) cube_uvs);
					mat_blue->bind_value(Shader::PARAM_VERT_COLORS, (void *) cube_colors);
					mat_blue->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) tex_gl_id);
					mat_blue->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);
				}
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *) 0);
			}
		}
	}
	//================================== End Drawing test boxes ==========================================

	Mat4 vp = camera->persp_proj_m * camera->view_m;

	//if(state.x > 0.95f && player_skel->playing_anim)
	//	player_skel->stop_anim();

	static bool played_anim = false;
	if(state.x < 0.10f)
	{
		if( player_skel->current_anim != 2 && !played_anim)
		{
			played_anim = true;
			player_skel->play_anim(2,Skeleton::END_TYPE_DEFAULT_ANIM);
		}
	}
	else
	{
		played_anim = false;
	}

	float t = time();

	//Make player spin
	//player->angles.y = fmodf(t*2.0f,TWO_PI);
	player->render(vp);

	//Making the test audio source rotate about the player
	//float distance = 5.0f + 2.0f * cosf(t*12.75f);
	float distance = 5.0f;
	//test_sound_source->pos = Vec3(distance * cosf(0.5f*t),distance * sinf(0.5f*t),0.0f);
	test_sound_source->pos = Vec3(0,distance,0);
	//test_sound_source->angles.y = fmodf(t*8.0f,TWO_PI);
	//test_sound_source->angles.x = fmodf(t*2.5f,TWO_PI);	makes cube tumble!
	//test_sound_source->angles.z = fmodf(t*3.0f,TWO_PI);	makes cube tumble!


	//=========== Test Audio Playing every 0.5 seconds ==============
	static float time_to_play_audio = 0.0f;
	if(t > time_to_play_audio)
	{
		time_to_play_audio = t + 0.5f;
		test_sound_source->play_sound(test_pulse);
	}
	Mat4 view_no_translation = camera->inf_proj_m * ((camera->view_m).pos_removed());
	skybox->render(view_no_translation);

	//Have to draw transparent sources after skybox
	test_sound_source->render(vp);


	//Test UI image
	//test_text->render(camera->ortho_proj_m);
	//test_img->render(camera->ortho_proj_m);

	eglSwapBuffers(egl_display, egl_surface);
}

float Engine::delta_time = 0.0f;
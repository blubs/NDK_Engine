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

	//============== Setting up game world objects/structs ======================
	///... how do I want to hold game structs?

	//camera = (Camera *) malloc(sizeof(Camera));
	camera = new Camera;
	camera->set_view_attributes(90.0f * DEG_TO_RAD, ASPECT_16_9_PORTRAIT, 0.01f, 1000.0f);

	player = new Player;
	//player = (Player*) malloc(sizeof(Player));

	cam_to_bone = new Entity_Bone_Joint;
	//cam_to_bone = (Entity_Bone_Joint*) malloc(sizeof(Entity_Bone_Joint));

	//=============== Setting up graphics objects (materials/shaders) ===========

	test_shader = (Shader *) malloc(sizeof(Shader));

	test_skeletal_shader = (Shader *) malloc(sizeof(Shader));

	mat_red = (Material *) malloc(sizeof(Material));
	mat_blue = (Material *) malloc(sizeof(Material));

	skeletal_mat = (Material *) malloc(sizeof(Material));

	mesh_shader = (Shader*) malloc(sizeof(Shader));
	mesh_mat = (Material*) malloc(sizeof(Material));

	test_arms = (Skel_Model*) malloc(sizeof(Skel_Model));

	test_texture = (Texture*) malloc(sizeof(Texture));

	text_shader = (Shader*) malloc(sizeof(Shader));
	text_mat = (Material*) malloc(sizeof(Material));
	test_text = (UI_Text*) malloc(sizeof(UI_Text));

	//player_skel = (Skeleton*) malloc(sizeof(Skeleton));
	player_skel = new Skeleton;
	//===========================================================================

	player->model = test_arms;
	player->skel = player_skel;


	camera->parent = cam_to_bone;
	cam_to_bone->parent_skel = player_skel;
	cam_to_bone->parent_bone_index = 8; //head bone is at index 8, we could add methods for finding the bone
	// but we don't need all of that at the moment (since camera is never going to be parented to anything but that bone)
	player_skel->parent = player;
}



void Engine::handle_cmd (struct android_app *app, int32_t cmd)
{
	Engine *eng = (Engine *) app->userData;

	switch(cmd)
	{
		//Saving our current state
		case APP_CMD_SAVE_STATE:
			eng->app->savedState = malloc(sizeof(struct saved_state));
			//*((struct saved_state*) eng->app->savedState) = *(eng->state);
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
			eng->start_audio();
			break;
			//App lost focus, stop monitoring acceleromter, and stop animating
		case APP_CMD_LOST_FOCUS:
			//if(eng->accelerometer_sensor != NULL)
			//{
			//	ASensorEventQueue_disableSensor(eng->sensor_event_queue,eng->accelerometer_sensor);
			//}
			eng->pause_audio();
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
		int32_t motion_action = AMotionEvent_getAction(event);
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
		}

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
	if(sl_audio_player_interface != NULL)
		start_audio();

#ifdef DEBUG_MODE
	LOGI("LIFECYCLE: init display finished.\n");
#endif

	return 0;
}

//========================================= Loading assets ======================================

int Engine::load_shaders ()
{
	test_shader->load("minimal.vert","minimal.frag");
	test_skeletal_shader->load("test_skeletal.vert","test_skeletal.frag");
	mesh_shader->load("minimal_mesh.vert","minimal_mesh.frag");
	text_shader->load("text.vert","text.frag");
	return 1;
}


//Returns 0 on fail, returns 1 on success.
int Engine::load_assets ()
{
	//Loading the test texture.
	test_texture->load("tex.pkm",512,512);

	test_arms->load_model("test_arms.skmf");
	test_arms->mat = mesh_mat;

	player_skel->load("player_skeleton.sksf");
	player_skel->load_animation("showcase_hands.skaf");
	player_skel->load_animation("speed_vault.skaf");

	test_arms->skel = player_skel;

	test_text->init(text_mat,test_texture);
	test_text->set_text("Hello World!");
	return 1;
}

//=================================================================================================

//========================================= Unloading assets ======================================
void Engine::unload_shaders ()
{
	test_shader->unload();
	test_skeletal_shader->unload();
	mesh_shader->unload();
	text_shader->unload();
}

void Engine::unload_assets ()
{
	if(test_texture)
		test_texture->unload();

	if(test_arms)
		test_arms->unload_model();

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
	pause_audio();
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

//Callback for swapping audio buffers
void sl_buffer_callback (SLBufferQueueItf snd_queue, void *c)
{
	Engine *e = ((Engine *) c);

	//Swap the audio buffers
	Stereo_Sample *other_buffer = e->active_audio_buffer;
	e->active_audio_buffer = e->inactive_audio_buffer;
	e->inactive_audio_buffer = other_buffer;

	//Wipe the current audio buffer
	memset(e->active_audio_buffer, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);

	//For sound effect interpolation
	//Last value to current value in SND_AUDIO_BUFFER_SIZE.
	//equation: lerped_effect = i*((cur_effect - last_effect)/SND_AUDIO_BUFFER_SIZE) + last_effect


	//Populate the current audio buffer with the whatever sounds that are playing.
	if(e->snd_ch.used)
	{
		float falloff = 1.0f / (50.0f * ((float) e->state.x) / ((float) e->width));
		falloff = fminf(1.0f, falloff);
		falloff = fmaxf(0.0f, falloff);
		float last_falloff = e->snd_ch.last_falloff;
		e->snd_ch.last_falloff = falloff;

		float faloff_slope = (falloff - last_falloff) / SND_AUDIO_BUFFER_SIZE;
		//Calculate "distance" falloff (our fingers x coordinate)
		//Distance emulated between 0 and 50 meters

		//Need file length, and audio
		int smpls_cp = SND_AUDIO_BUFFER_SIZE < (e->snd_ch.length - e->snd_ch.position) ? SND_AUDIO_BUFFER_SIZE :
		(e->snd_ch.length - e->snd_ch.position);


		for(int i = 0; i < smpls_cp; i++)
		{
			//Calculating current lerped falloff
			falloff = faloff_slope * i + last_falloff;

			Stereo_Sample smp = *((Stereo_Sample *) (e->snd_ch.data) + (i + e->snd_ch.position));
			smp.l *= falloff;
			smp.r *= falloff;
			e->active_audio_buffer[i] = smp;
		}
		e->snd_ch.position += smpls_cp;
		if((e->snd_ch.position) >= e->snd_ch.length)
		{
			e->snd_ch.used = false;
		}
	}
	//Send the prepared audio buffer
	(*(snd_queue))->Enqueue(snd_queue, e->active_audio_buffer, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
}

void Engine::play_sound ()
{
	if(snd_ch.data == NULL)
		return;

	snd_ch.used = true;
	snd_ch.position = 0;
}

int Engine::init_sl ()
{

	//=================================== Creating the SL Sound Engine ======================================
	const SLuint32 eng_mix_iid_count = 1;
	const SLInterfaceID eng_mix_iids[] = {SL_IID_ENGINE};
	const SLboolean eng_mix_reqs[] = {SL_BOOLEAN_TRUE};

	SLresult result;
	result = slCreateEngine(&sl_engine, 0, NULL, eng_mix_iid_count, eng_mix_iids, eng_mix_reqs);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("slCreateEngine failed");
		return 0;
	}

	result = (*sl_engine)->Realize(sl_engine, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine Realize failed");
		return 0;
	}

	result = (*sl_engine)->GetInterface(sl_engine, SL_IID_ENGINE, &sl_engine_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine GetInterface failed");
		return 0;
	}
	//======================================================================================================

	//======================================= Creating the Output Mix object ===============================

	const SLuint32 out_mix_iid_count = 0;
	const SLInterfaceID out_mix_iid[] = {};
	const SLboolean out_mix_req[] = {};

	result = (*sl_engine_interface)->CreateOutputMix(sl_engine_interface, &sl_output_mix, out_mix_iid_count, out_mix_iid,
										    out_mix_req);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine CreatOutputMix failed");
		return 0;
	}
	result = (*sl_output_mix)->Realize(sl_output_mix, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine output_mix_object Realize failed");
		return 0;
	}

	//======================================================================================================


	//============================================= Setting up Audio Player ================================

	//============================ Setting up data sources =================================================

	SLDataLocator_AndroidSimpleBufferQueue bufq_loc;
	bufq_loc.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	bufq_loc.numBuffers = 2;

	//Format of the audio data
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = 2;//1 for mono audio, 2 for stereo audio
	format_pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
	format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
	format_pcm.channelMask = SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_LEFT;//SL_SPEAKER_FRONT_CENTER for mono audio
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	//Setting up the audio data source input
	SLDataSource audio_source = {&bufq_loc, &format_pcm};
	audio_source.pLocator = &bufq_loc;
	audio_source.pFormat = &format_pcm;

	//Setting up the audio data source output
	SLDataLocator_OutputMix data_locator_out;
	data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	data_locator_out.outputMix = sl_output_mix;

	SLDataSink data_sink;
	data_sink.pLocator = &data_locator_out;
	data_sink.pFormat = NULL;
	//=================================================================================================

	//================================== Creating the Sound player ====================================
	const SLuint32 snd_plyr_iid_count = 3;
	const SLInterfaceID snd_plyr_iids[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean snd_plyr_reqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


	result = (*sl_engine_interface)->CreateAudioPlayer(sl_engine_interface, &sl_audio_player, &audio_source, &data_sink,
											 snd_plyr_iid_count, snd_plyr_iids, snd_plyr_reqs);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_engine CreateAudioPlayer failed");
		return 0;
	}
	result = (*sl_audio_player)->Realize(sl_audio_player, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player Realize failed");
	}
	//Getting the three interfaces we requested above

	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_PLAY, &sl_audio_player_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface play failed");
		return 0;
	}
	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_BUFFERQUEUE, &sl_buffer_queue_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface buffer queue failed");
		return 0;
	}
	result = (*sl_audio_player)->GetInterface(sl_audio_player, SL_IID_VOLUME, &sl_volume_interface);
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_audio_player GetInterface volume failed");
		return 0;
	}
	//===========================================================================================================
	//=============== Setting the Buffer Swapping Callback ======================================================
	// called when current buffer is done playing to prepare the next buffer
	result =
	(*sl_buffer_queue_interface)->RegisterCallback(sl_buffer_queue_interface, sl_buffer_callback, this);//(void*)&osl_engine
	if(result != SL_RESULT_SUCCESS)
	{
		LOGE("sl_buffer_queue_interface RegisterCallback failed");
		return 0;
	}
	//===========================================================================================================


	//===================================== Everything after this is for testing ================================
	//Filling audio buffers with all 0s
	memset(audio_buffer1, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);
	memset(audio_buffer2, 0, sizeof(Stereo_Sample) * SND_AUDIO_BUFFER_SIZE);

	//Putting a test sine wave on the buffers.
	/*for(int i = 0; i < SND_AUDIO_BUFFER_SIZE; i++)
	{
		audio_buffer1[i].l = (short) (15000 * sin(i * 0.4));
		//Setting the second buffer to have a sine wave that begins at the end of the first buffer's
		audio_buffer2[i].l = (short) (15000 * sin((i + SND_AUDIO_BUFFER_SIZE) * 0.4));

		//Giving the right channel a different sine wave
		audio_buffer1[i].r = (short) (15000 * sin(i * 0.6));
		//Setting the second buffer to have a sine wave that begins at the end of the first buffer's
		audio_buffer2[i].r = (short) (15000 * sin((i + SND_AUDIO_BUFFER_SIZE) * 0.6));
	}*/

	//Point to the first buffer
	active_audio_buffer = audio_buffer1;
	inactive_audio_buffer = audio_buffer2;

	//this is where buffer is wiped to all 0's, audio clips are placed in buffer, etc.
	//Send first audio buffer to the audio player
	(*sl_buffer_queue_interface)->Enqueue(sl_buffer_queue_interface, active_audio_buffer, sizeof(short) * SND_AUDIO_BUFFER_SIZE);
	//Swap the audio buffers
	Stereo_Sample *other_buffer = active_audio_buffer;
	active_audio_buffer = inactive_audio_buffer;
	inactive_audio_buffer = other_buffer;

	start_audio();

	//TODO: iterate through sound structs setting used to false
	//Setting buffers as not in use
	//	for(int i = 0; i < SND_AUDIO_CHANNELS; i++)
	//	{
	//		osl_engine.mSounds[i].mUsed = false;
	//	}
}

void Engine::term_sl ()
{
	stop_audio();
	if(sl_audio_player != NULL)
	{
		SLuint32 sound_player_state;
		(*sl_audio_player)->GetState(sl_audio_player, &sound_player_state);

		if(sound_player_state == SL_OBJECT_STATE_REALIZED)
		{
			(*sl_buffer_queue_interface)->Clear(sl_buffer_queue_interface);
			(*sl_audio_player)->AbortAsyncOperation(sl_audio_player);
			(*sl_audio_player)->Destroy(sl_audio_player);
			sl_audio_player = NULL;
			sl_audio_player_interface = NULL;
			sl_buffer_queue_interface = NULL;
			sl_volume_interface = NULL;
		}
	}
	//Destroying the output mix object
	if(sl_output_mix != NULL)
	{
		(*sl_output_mix)->Destroy(sl_output_mix);
		sl_output_mix = NULL;
	}

	//Destroying the sound engine
	if(sl_engine != NULL)
	{
		(*sl_engine)->Destroy(sl_engine);
		sl_engine = NULL;
		sl_engine_interface = NULL;
	}
}

void Engine::start_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_PLAYING);
}

void Engine::stop_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_STOPPED);
}

void Engine::pause_audio ()
{
	(*sl_audio_player_interface)->SetPlayState(sl_audio_player_interface, SL_PLAYSTATE_PAUSED);
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

	mat_red->initialize();
	mat_red->set_shader(test_shader);
	float color_red[] = {1.0f, 0.4f, 0.4f, 1.0f};
	mat_red->set_fixed_shader_param(Shader::PARAM_TEST_FIELD, color_red, sizeof(float) * 4);

	mat_blue->initialize();
	mat_blue->set_shader(test_shader);
	float color_blue[] = {0.4f, 0.4f, 1.0f, 1.0f};
	mat_blue->set_fixed_shader_param(Shader::PARAM_TEST_FIELD, color_blue, sizeof(float) * 4);


	//================================Initializing the skeletal shaders======================================
	GLuint skel_param_types[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_VERT_UV1,
		Shader::PARAM_TEXTURE_DIFFUSE,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_BONE_MATRICES,
		Shader::PARAM_BONE_INDICES,
		Shader::PARAM_BONE_WEIGHTS
	};
	const char *skel_param_names[] =
	{
		"vert_pos",
		"src_tex_coord",
		"tex",
		"mvp",
		"bone",
		"bone_index",
		"bone_weight"
	};
	test_skeletal_shader->init_gl(skel_param_types, skel_param_names, 7);

	skeletal_mat->initialize();
	skeletal_mat->set_shader(test_skeletal_shader);


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

	text_mat->initialize();
	text_mat->set_shader(text_shader);

	//========================================= Initializing the mesh shader ================================

	GLuint mesh_shader_param_types[] =
	{
		Shader::PARAM_VERTICES,
		Shader::PARAM_MVP_MATRIX,
		Shader::PARAM_BONE_INDICES,
		Shader::PARAM_BONE_WEIGHTS,
		Shader::PARAM_BONE_MATRICES
	};
	const char *mesh_shader_param_names[] =
	{
		"vert_pos",
		"mvp",
		"bone_index",
		"bone_weight",
		"bone"
	};
	mesh_shader->init_gl(mesh_shader_param_types, mesh_shader_param_names, 5);

	mesh_mat->initialize();
	mesh_mat->set_shader(mesh_shader);


	//==================================== Loading textures =======================================
	test_texture->init_gl();

	//==================================== Setting up Mesh VBOs ====================================
	test_arms->init_gl();
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
	//Terminating all loaded materials
	mat_blue->term();
	mat_red->term();
	mesh_mat->term();
	skeletal_mat->term();
	text_mat->term();
	//Terminating all loaded shaders
	test_shader->term_gl();
	test_skeletal_shader->term_gl();
	mesh_shader->term_gl();
	text_shader->term_gl();


	//Terminating all loaded models
	test_arms->term_gl();




	test_texture->term_gl();

	gl_initialized = false;
}

int Engine::init_data ()
{
	LOGI("init_data...\n");
	if(!init_sl())
		return 0;
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
	term_sl();
	data_initialized = false;
}


//Destroys all data allocated by constructor
void Engine::term()
{
	if(test_shader)
		free(test_shader);

	if(test_skeletal_shader)
		free(test_skeletal_shader);

	if(mat_red)
		free(mat_red);

	if(mat_blue)
		free(mat_blue);

	if(skeletal_mat)
		free(skeletal_mat);

	if(mesh_shader)
		free(mesh_shader);

	if(mesh_mat)
		free(mesh_mat);

	if(text_mat)
		free(text_mat);
	if(text_shader)
		free(text_shader);

	if(test_arms)
		free(test_arms);

	if(test_texture)
		free(test_texture);

	if(player_skel)
		delete player_skel;
	//	free(player_skel);
	if(player)
		delete player;
	if(camera)
		delete camera;
	if(cam_to_bone)
		delete cam_to_bone;
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	//Filling the screen with a color
	glClearColor(state.x, 0.0f/*state.angle*/, state.y, 1);



	//Setting all transforms to be recalculated
	player_skel->transform_calculated = false;
	camera->transform_calculated = false;
	player->transform_calculated = false;
	cam_to_bone->transform_calculated = false;


	//glClear(GL_COLOR_BUFFER_BIT);

	//Triangle is in the xy plane, facing the negative y direction
	/*const float triangleVertices[] =
	{
		-0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, -0.5f,
		0.5f, 0.0f, 0.5f, //For triangle in 3 space
		//-0.5f, 0.5f, 0.0f,
		//0.0f, -0.5f, 0.0f,
		//0.5f, 0.5f, 0.0f, //For triangle in 2 space
	};
	const float triangleUVs[] =
	{
		0.25f, 0.0f,
		0.5f, 1.0f,
		0.75f, 0.0f,
	};
	const float triangleColors[] =
	{
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
	};*/

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

	//int vert_count = 36;
	int vert_count = 8;


	//Creating a model matrix, whose rotation is dictated by the state.x and state.y
	//Quat yaw(((state.x*2.0f)-1.0f) * HALF_PI*0.5f,Vec3::UP());
	//Quat pitch(((state.y*2.0f)-1.0f) * HALF_PI*0.5f,(yaw*Vec3::RIGHT()));
	//Quat rot = pitch*yaw;
	//Mat4 model_rot = Mat4::ROTATE(rot);

	camera->pos = Vec3::ZERO();

	camera->angles = Vec3::ZERO();
	//Pitch
	camera->angles.x = ((0.5f - state.y) * TWO_PI);
	//Yaw
	camera->angles.y = (0.5f - state.x) * TWO_PI;
	//Roll
	camera->angles.z = 0.0f;
	camera->update_view_matrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(gl_program);



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

	//Have to bind the data before a call (taken care of above)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	/*test_shader->bind_shader();
	test_shader->bind_shader_value(Shader::PARAM_VERTICES,(void*)cube_vertices);
	test_shader->bind_shader_value(Shader::PARAM_VERT_UV1,(void*)cube_uvs);
	test_shader->bind_shader_value(Shader::PARAM_VERT_COLORS,(void*)cube_colors);
	test_shader->bind_shader_value(Shader::PARAM_TEXTURE_DIFFUSE,(void*)texture_id);*/

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
				Mat4 mvp = camera->projection_m * camera->view_m * model_transform;

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
				//glDrawArrays(GL_TRIANGLES, 0, vert_count);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *) 0);
			}
		}
	}


	//Here is where we are going to test skeletal animation shader.

	//This model has 2 vertically stretched boxes, which are weighted to different bones.
	const float joint_vertices[] =
	{
		//Front quad
		-0.1f, 0.1f, 1.0f,
		0.1f, 0.1f, 1.0f,
		0.1f, 0.1f, 0.1f,
		-0.1f, 0.1f, 0.1f,
		//Back quad
		0.1f, -0.1f, 1.0f,
		-0.1f, -0.1f, 1.0f,
		-0.1f, -0.1f, 0.1f,
		0.1f, -0.1f, 0.1f,
		//Right quad
		//Left quad
		//Top quad
		//Bottom quad

		//Second box
		-0.1f, 0.1f, -0.1f,
		0.1f, 0.1f, -0.1f,
		0.1f, 0.1f, -1.0f,
		-0.1f, 0.1f, -1.0f,
		0.1f, -0.1f, -0.1f,
		-0.1f, -0.1f, -0.1f,
		-0.1f, -0.1f, -1.0f,
		0.1f, -0.1f, -1.0f
	};

	//Referencing vertices by index
	unsigned int joint_tris[] =
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

		//Second box
		//Front quad
		8, 9, 10, 10, 11, 8,
		//Back quad
		12, 13, 14, 14, 15, 12,
		//Right quad
		9, 12, 15, 15, 10, 9,
		//Left quad
		8, 11, 14, 14, 13, 8,
		//Top quad
		12, 9, 8, 8, 13, 12,
		//Bottom quad
		14, 11, 10, 10, 15, 14
	};

	GLuint element_buffer2;
	glGenBuffers(1, &element_buffer2);
	//Bind the buffer to set the data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 72 * sizeof(unsigned int)/*size of indices*/, joint_tris, GL_STATIC_DRAW);


	const float joint_uvs[] =
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

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	//All vertices must have valid indices for 3 bones,
	// since we do not check in the shader whether an index is valid or not
	// however, the respective weight of unused bones for a vert should be 0.0f,
	// thereby negating the bone's influence on the vertex
	const float joint_bone_indices[] =
	{
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	//First 8 verts are weighted by both bones
	//Last 8 verts are only weighted by the first bone
	const float joint_bone_weights[] =
	{
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,
		0.9848f,0.1736f,0.0f,

		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f
	};


	Vec3 pos(0,7,0);

	Mat4 model_pos = Mat4::TRANSLATE(pos);
	Mat4 model_transform = model_pos;
	Mat4 vp = camera->projection_m * camera->view_m;
	Mat4 mvp = vp * model_transform;

	int BONE_COUNT = 2;
	float* joint_matrices = (float*) malloc(sizeof(float) * (BONE_COUNT*16));
	Mat4 id = Mat4::IDENTITY();

	static float angle = 0.0f;
	angle += 0.1f;
	if(angle > 360.0f)
		angle = 0.0f;

	Quat rot(angle,Vec3::FRONT());
	Mat4 rot_matrix = Mat4::ROTATE(rot);


	//Copying identity matrices
	for(i = 0; i < 16; i++)
	{
		//The first bone's matrix
		joint_matrices[i] = id.m[i];
		//The second bone's matrix
		joint_matrices[16+i] = rot_matrix.m[i];
	}

	skeletal_mat->bind_material();
	skeletal_mat->bind_value(Shader::PARAM_VERTICES, (void*) joint_vertices);
	skeletal_mat->bind_value(Shader::PARAM_VERT_UV1, (void*) joint_uvs);
	skeletal_mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void*) tex_gl_id);
	skeletal_mat->bind_value(Shader::PARAM_MVP_MATRIX, (void*) mvp.m);
	skeletal_mat->bind_value(Shader::PARAM_BONE_INDICES,(void*)joint_bone_indices);
	skeletal_mat->bind_value(Shader::PARAM_BONE_WEIGHTS,(void*)joint_bone_weights);
	skeletal_mat->bind_values(Shader::PARAM_BONE_MATRICES,joint_matrices,BONE_COUNT);

	//glDrawArrays(GL_TRIANGLES, 0, vert_count);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, (void *) 0);

	if(state.x > 0.95f && player_skel->playing_anim)
		player_skel->stop_anim();
	if(state.x < 0.05f && !player_skel->playing_anim)
		player_skel->play_anim(0);
	player->render(vp);
	test_text->render(Mat4::IDENTITY());//Identity for drawing in view space

	eglSwapBuffers(egl_display, egl_surface);
}

float Engine::delta_time = 0.0f;
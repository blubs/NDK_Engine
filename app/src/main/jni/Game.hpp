//
// Created by F1 on 8/2/2016.
//

#ifndef ENGINE_GAME_HPP
#define ENGINE_GAME_HPP

#include "engine/common.hpp"
#include "entities/Camera.hpp"
#include "entities/Player.hpp"
#include "engine/graphics/UI.hpp"
#include "engine/graphics/Skybox.hpp"
#include "engine/sound/Sound_Source.hpp"


class Game
{
public:

	Game()
	{
	}

	~Game()
	{
	}

	//Variables that engine uses
	Camera* audio_listener;
	int32_t screen_width;
	int32_t screen_height;

	//=========================== Game Asset Variables ====================
	//-------------- Sound Files --------------
	Sound_Sample* test_pulse = NULL;

	//-------------- Shaders ------------------
	Shader* test_shader = NULL;
	Shader* skel_color_shader = NULL;
	Shader* static_color_shader = NULL;
	Shader* text_shader = NULL;

	//------------- Materials -----------------
	Material* mat_red = NULL;
	Material* mat_blue = NULL;
	Material* skel_color_mat= NULL;
	Material* static_color_mat = NULL;
	Material* text_mat = NULL;

	//------------ Textures -------------------
	Texture* test_texture = NULL;
	Texture* char_set = NULL;
	Cube_Map* test_cube_map = NULL;

	//------------- Models --------------------
	Skel_Model* test_arms = NULL;
	Static_Model* model_prim_cube = NULL;
	Static_Model* model_prim_quad = NULL;

	//---------- Skeletons --------------------
	Skeleton* player_skel = NULL;

	//=========================== Game Object Variables ====================
	Camera* camera = NULL;
	Player* player = NULL;
	Entity_Bone_Joint* cam_to_bone = NULL;

	Entity* test_sound_source = NULL;

	UI_Text* test_text = NULL;
	UI_Image* test_img = NULL;

	Skybox* skybox = NULL;

	//======================================================================

	//========================================= Loading assets ======================================

	int load_shaders ()
	{
		test_shader = new Shader("minimal.vert","minimal.frag");
		skel_color_shader = new Shader("skeletal_color.vert","skeletal_color.frag");
		static_color_shader = new Shader("static_color.vert","static_color.frag");
		text_shader = new Shader("monochrome_transparent.vert","monochrome_transparent.frag");
		return 1;
	}
	void unload_shaders ()
	{
		if(test_shader)
			delete test_shader;
		if(skel_color_shader)
			delete skel_color_shader;
		if(static_color_shader)
			delete static_color_shader;
		if(text_shader)
			delete text_shader;
	}

	int load_materials()
	{
		mat_red = new Material();
		mat_blue = new Material();
		skel_color_mat = new Material();
		static_color_mat = new Material();
		text_mat = new Material();
	}

	void unload_materials()
	{
		if(mat_red)
			delete mat_red;
		if(mat_blue)
			delete mat_blue;
		if(skel_color_mat)
			delete skel_color_mat;
		if(text_mat)
			delete text_mat;
	}

	int load_textures()
	{
		test_texture = new Texture("tex.pkm",512,512);
		char_set = new Texture("char_set.pkm",2048,2048);
		test_cube_map = (Cube_Map*) malloc(sizeof(Cube_Map));

		//TODO: clean this up
		test_cube_map->load("cube_maps/test_cube_map.pkm",512);
	}
	void unload_textures()
	{
		if(test_texture)
			delete test_texture;
		if(char_set)
			delete char_set;
		//TODO: clean this up

		if(test_cube_map)
			test_cube_map->unload();
		if(test_img)
			free(test_img);
	}

	int load_models()
	{
	}

	void unload_models()
	{
	}

	int load_assets ()
	{

		/*	//===================== Setting up game world objects =======================


	//===================== Creating Skeletal Models ============================
	test_arms = (Skel_Model*) malloc(sizeof(Skel_Model));

	//================== Instantiating Static Models ============================
	model_prim_cube = (Static_Model*) malloc(sizeof(Static_Model));
	model_prim_quad = (Static_Model*) malloc(sizeof(Static_Model));

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

	//=================== Setting up temp sound containers ====================
	test_pulse = (Sound_Sample*) malloc(sizeof(Sound_Sample));

	//===========================================================================*/


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


	void unload_assets ()
	{
		/*
	if(test_pulse)
		free(test_pulse);

	//Freeing models
	if(test_arms)
		free(test_arms);
	if(model_prim_cube)
		free(model_prim_cube);
	if(model_prim_quad)
		free(model_prim_quad);

	//Deleting other objects
	//	if(test_text)
	//		free(test_text);

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
		delete skybox;*/
		//Sounds
		if(test_pulse)
			test_pulse->unload();
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
	//Initialize GL aspects of all assets
	int init_gl()
	{
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
	}

	void term_gl()
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
	}

	//Ran on first frame
	//This is where we set up our game objects and their relationships
	void start()
	{
		//TODO: create game objects here
		audio_listener = camera;


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
		test_text->pos.x = -screen_width * 0.4f;
		test_text->pos.y = screen_height * 0.4f;

		test_img->init(text_mat,test_texture);
		test_img->pos.x = screen_width*0.5f - 100.0f;
		test_img->pos.y = screen_height*0.5f - 100.0f;
		test_img->scale.x = 200.0f;
		test_img->uv_maxs.x = 0.6f;
		test_img->uv_mins.y = 0.5f;
		test_img->maintain_aspect_ratio = true;

		player->player_model = test_arms;
		player->skel = player_skel;


		camera->parent = cam_to_bone;
		cam_to_bone->parent_skel = player_skel;
		cam_to_bone->parent_bone_index = 8; //head bone is at index 8, we could add methods for finding the bone
		// but we don't need all of that at the moment (since we are never going to parent anything to any other bone)
		player_skel->parent = player;
		camera->set_persp_view(90.0f * DEG_TO_RAD, screen_width,screen_height, 0.01f, 1000.0f);
		camera->set_ortho_view(screen_width,screen_height,0.0001f,1.0f);

	}

	//Ran on last frame
	//This is where we destroy our game objects
	void finish()
	{
		test_text->term();
		test_img->term();
	}

	//Updates the game state / logic
	void update()
	{
		//TODO
	}

	//Draws the scene
	void render()
	{
		//TODO
	}

};

#endif //ENGINE_GAME_HPP

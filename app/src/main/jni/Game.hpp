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

		return 1;
	}
	void unload_materials()
	{
		delete mat_red;
		delete mat_blue;
		delete skel_color_mat;
		delete text_mat;
	}

	int load_textures()
	{
		test_texture = new Texture("tex.pkm",512,512);
		char_set = new Texture("char_set.pkm",2048,2048);
		test_cube_map = new Cube_Map("cube_maps/test_cube_map.pkm",512);
		return 1;
	}
	void unload_textures()
	{
		delete test_texture;
		delete char_set;
		delete test_cube_map;
	}

	int load_models()
	{
		test_arms = new Skel_Model("test_arms.skmf");

		model_prim_cube = new Static_Model("primitive_cube.stmf");
		model_prim_quad = new Static_Model("primitive_quad.stmf");

		player_skel = new Skeleton("player_skeleton.sksf");
		player_skel->load_animation("player_animations/run.skaf");
		player_skel->load_animation("player_animations/showcase_hands.skaf");
		player_skel->load_animation("player_animations/speed_vault.skaf");

		skybox = new Skybox();
		return 1;
	}
	void unload_models()
	{
		delete test_arms;

		delete model_prim_cube;
		delete model_prim_quad;

		delete player_skel;

		delete skybox;
	}

	int load_sounds()
	{
		test_pulse = new Sound_Sample("test_audio_pulse.raw");
		return 1;
	}
	void unload_sounds()
	{
		delete test_pulse;
	}

	int load_assets ()
	{
		if(!load_textures())
			return 0;
		if(!load_shaders())
			return 0;
		if(!load_materials())
			return 0;
		if(!load_models())
			return 0;
		if(!load_sounds())
			return 0;
		return 1;
	}
	void unload_assets ()
	{
		unload_sounds();
		unload_models();
		unload_materials();
		unload_shaders();
		unload_textures();
	}
	//=================================================================================================
	//Initialize GL aspects of all assets
	int init_gl()
	{
		//Initializing shaders
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
		test_shader->init_gl(param_types, param_names,6);

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
		text_shader->init_gl(text_param_types, text_param_names,6);

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
		return 1;
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
		camera = new Camera();
		player = new Player();
		cam_to_bone = new Entity_Bone_Joint();
		test_sound_source = new Entity();

		//===== Setting up relationships between game objects ======
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


		//===== Instantiating Game Objects =====
		test_text = new UI_Text(text_mat,char_set);
		//test_text->set_text("test\nT  E\n\nST !@\n#$%^&*()");
		test_text->set_text("Pause\npause\nPAUSE\n\nPlay\nplay\nPLAY\n\nExit\nexit\nEXIT\n\ntest_text->set_text(\"Stuff\")");
		//Place in top leftish corner
		test_text->pos.x = -screen_width * 0.4f;
		test_text->pos.y = screen_height * 0.4f;


		test_img = new UI_Image(text_mat,test_texture);
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
		delete test_text;
		delete test_img;

		delete player;
		delete test_sound_source;
		delete camera;
		delete cam_to_bone;
	}

	//Input static identifiers
	const static int INPUT_EVENT_ON_TOUCH_DOWN = 1;
	const static int INPUT_EVENT_ON_TOUCH_MOVE = 2;
	const static int INPUT_EVENT_ON_TOUCH_RELEASE = 3;

	//Temp test input variables
	float input_x;
	float input_y;

	int handle_input(float x, float y, int event)
	{
		switch(event)
		{
			case INPUT_EVENT_ON_TOUCH_DOWN:
			case INPUT_EVENT_ON_TOUCH_MOVE:
			case INPUT_EVENT_ON_TOUCH_RELEASE:
				input_x = x;
				input_y = y;
				break;
		}
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

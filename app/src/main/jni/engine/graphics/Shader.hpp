//
// Created by F1 on 5/16/2016.
//
#include "../common.hpp"
#include "GL_Utils.hpp"

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H


class Shader
{
public:
	//Holding the raw src code of the shaders
	const char *vert_shader_src;
	const char *frag_shader_src;
	char *vert_shader_name;
	char *frag_shader_name;

	//we construct a shader by passing in the shader source files
	GLuint gl_program = 0;

	GLuint vert_shader;
	GLuint frag_shader;

	//Static types for parameters
	static const int PARAM_VERTICES = 1;
	static const int PARAM_VERT_NORMALS = 2;
	static const int PARAM_VERT_TANGENTS = 3;
	static const int PARAM_VERT_BINORMALS = 4;
	static const int PARAM_VERT_COLORS = 5;
	static const int PARAM_VERT_UV1 = 6;
	static const int PARAM_VERT_UV2 = 7;
	static const int PARAM_MVP_MATRIX = 8;
	static const int PARAM_M_IT_MATRIX = 9;
	static const int PARAM_TEXTURE_DIFFUSE = 10;
	static const int PARAM_TEXTURE_NORMAL = 11;
	static const int PARAM_TEXTURE_MISC = 12;
	static const int PARAM_TEXTURE_LIGHTMAP = 13;
	static const int PARAM_BONE_MATRICES = 14;
	static const int PARAM_BONE_IT_MATRICES = 15;
	static const int PARAM_BONE_INDICES = 16;
	static const int PARAM_BONE_WEIGHTS = 17;
	static const int PARAM_COLOR_MULT = 18;
	static const int PARAM_COLOR_ADD = 19;
	static const int PARAM_CUBE_MAP = 20;
	static const int PARAM_TEST_FIELD = 21;
	//TODO: any other parameter types



	//Arrays that hold arbitrary types of parameters
	void **param_location;
	GLuint *param_type;
	uint param_count;

	//Running count of how many textures we have bound. Resets every time bind is initiated.
	int bound_textures;

	//Constructs and loads shader
	Shader(const char *vshader_name, const char *fshader_name);
	//Destroys and unloads shader
	~Shader();

	//Initializes the shader given vertex / fragment shader src and name, as well as a list of parameter types and values (including the number of parameters)
	int init_gl(GLuint *param_types, const char **param_identifiers, uint params_count);

	//Freeing the shader and allocated memory
	void term_gl();
	//Binds a shader for use by renderer (should be called before binding individual values)
	int bind_shader();

	//Binds a value to a shader location for rendering
	int bind_shader_value(GLuint type,void *data, int extra_data);

	//Same as above if we already know what index the data goes in.
	int bind_shader_value_by_index (int index, void *data, int extra_data);

private:
	//Loads the raw shader source
	int load(const char *vshader_name, const char *fshader_name);

	//Frees the loaded raw shader source
	void unload();

public:
	//The following section is for static global variables that shaders access if the source includes reference to them
	//A list of the uniform locations of the global parameters, if a location is set, the shader uses that parameter
	GLint* global_param_location = NULL;

	//Global parameters
	static const int GLOBAL_PARAM_FLOAT_TIME = 0;
	static const int GLOBAL_PARAM_VEC3_CAM_POS = 1;
	static const int GLOBAL_PARAM_VEC3_CAM_DIR = 2;
	static const int GLOBAL_PARAM_VEC3_DIRLIGHT_DIR = 3;
	static const int GLOBAL_PARAM_VEC3_DIRLIGHT_COL = 4;
	//Removed matrix global parameters: highly doubt I'll actually make use of them
	/*static const int GLOBAL_PARAM_MATRIX_VP = 5;
	static const int GLOBAL_PARAM_MATRIX_V = 6;
	static const int GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF = 7;//For skybox
	static const int GLOBAL_PARAM_MATRIX_P_PERSP = 8;
	static const int GLOBAL_PARAM_MATRIX_P_ORTHO = 9;
	static const int GLOBAL_PARAM_MATRIX_P_INF = 10;*/

	//static const int GLOBAL_PARAM_COUNT = 11;//Parameter count including matrices
	static const int GLOBAL_PARAM_COUNT = 5;//Parameter count excluding matrices

	static const char *GLOBAL_PARAM_FLOAT_TIME_ID = "time";
	static const char *GLOBAL_PARAM_VEC3_CAM_POS_ID = "cam_pos";
	static const char *GLOBAL_PARAM_VEC3_CAM_DIR_ID = "cam_dir";
	static const char *GLOBAL_PARAM_VEC3_DIRLIGHT_DIR_ID = "dirlight_dir";
	static const char *GLOBAL_PARAM_VEC3_DIRLIGHT_COL_ID = "dirlight_col";
	/*static const char *GLOBAL_PARAM_MATRIX_VP_ID = "mat_vp";
	static const char *GLOBAL_PARAM_MATRIX_V_ID = "mat_v";
	static const char *GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF_ID = "mat_vp_skybox";//For skybox
	static const char *GLOBAL_PARAM_MATRIX_P_PERSP_ID = "mat_p_persp";
	static const char *GLOBAL_PARAM_MATRIX_P_ORTHO_ID = "mat_p_ortho";
	static const char *GLOBAL_PARAM_MATRIX_P_INF_ID = "mat_p_inf";*/

	static const char *GLOBAL_PARAM_IDS[] =
	{
		GLOBAL_PARAM_FLOAT_TIME_ID,
		GLOBAL_PARAM_VEC3_CAM_POS_ID,
		GLOBAL_PARAM_VEC3_CAM_DIR_ID,
		GLOBAL_PARAM_VEC3_DIRLIGHT_DIR_ID,
		GLOBAL_PARAM_VEC3_DIRLIGHT_COL_ID
		/*,GLOBAL_PARAM_MATRIX_VP_ID,
		GLOBAL_PARAM_MATRIX_V_ID,
		GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF_ID,
		GLOBAL_PARAM_MATRIX_P_PERSP_ID,
		GLOBAL_PARAM_MATRIX_P_ORTHO_ID,
		GLOBAL_PARAM_MATRIX_P_INF_ID*/
	};

	static void** global_params = NULL;

	//Checks if any of the global params exists in the shader source
	void search_for_global_params()
	{
		for(int i = 0; i < GLOBAL_PARAM_COUNT; i++)
		{
			global_param_location[i] = -1;
			global_param_location[i] = glGetUniformLocation(gl_program, GLOBAL_PARAM_IDS[i]);
		}

	}
	//TODO: a method for unbinding used global params?
		//this would be required because we search on init_gl, so we should destroy on term_gl
	//Binds any of the global params that this shader uses
	void bind_used_global_params()
	{
		GLint loc = -1;
		for(int type = 0; type < GLOBAL_PARAM_COUNT; type++)
		{
			if(global_param_location[type] == -1)
				continue;

			loc = global_param_location[type];

			//TODO: bind the global param to the shader
			switch(type)
			{
				//16 floats
				/*case GLOBAL_PARAM_MATRIX_VP:
				case GLOBAL_PARAM_MATRIX_V:
				case GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF:
				case GLOBAL_PARAM_MATRIX_P_PERSP:
				case GLOBAL_PARAM_MATRIX_P_ORTHO:
				case GLOBAL_PARAM_MATRIX_P_INF:
					glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)(global_params[type]));
				 	break;*/
				//3 floats
				case GLOBAL_PARAM_VEC3_CAM_POS:
				case GLOBAL_PARAM_VEC3_CAM_DIR:
				case GLOBAL_PARAM_VEC3_DIRLIGHT_DIR:
				case GLOBAL_PARAM_VEC3_DIRLIGHT_COL:
					glUniform3f(loc, ((float*)(global_params[type]))[0], ((float*)(global_params[type]))[1], ((float*)(global_params[type]))[2]);
					break;
				//One float
				case GLOBAL_PARAM_FLOAT_TIME:
					glUniform1f(loc,((float*)(global_params[type]))[0]);
					break;
			}
		}
	}

	//Sets static global params to be accessed by all shaders
		//type is the global param to set
		//value is a pointer to 1,3, or 16 float values
	static void set_static_global_param(int type,void *value)
	{
		switch(type)
		{
			//16 floats
			/*case GLOBAL_PARAM_MATRIX_VP:
			case GLOBAL_PARAM_MATRIX_V:
			case GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF:
			case GLOBAL_PARAM_MATRIX_P_PERSP:
			case GLOBAL_PARAM_MATRIX_P_ORTHO:
			case GLOBAL_PARAM_MATRIX_P_INF:
				((float*)(global_params[type]))[15] = ((float*)value)[15];
				((float*)(global_params[type]))[14] = ((float*)value)[14];
				((float*)(global_params[type]))[13] = ((float*)value)[13];
				((float*)(global_params[type]))[12] = ((float*)value)[12];
				((float*)(global_params[type]))[11] = ((float*)value)[11];
				((float*)(global_params[type]))[10] = ((float*)value)[10];
				((float*)(global_params[type]))[9] = ((float*)value)[9];
				((float*)(global_params[type]))[8] = ((float*)value)[8];
				((float*)(global_params[type]))[7] = ((float*)value)[7];
				((float*)(global_params[type]))[6] = ((float*)value)[6];
				((float*)(global_params[type]))[5] = ((float*)value)[5];
				((float*)(global_params[type]))[4] = ((float*)value)[4];
				((float*)(global_params[type]))[3] = ((float*)value)[3];
					//FALLTHROUGH for the first 3 indices*/
			//3 floats
			case GLOBAL_PARAM_VEC3_CAM_POS:
			case GLOBAL_PARAM_VEC3_CAM_DIR:
			case GLOBAL_PARAM_VEC3_DIRLIGHT_DIR:
			case GLOBAL_PARAM_VEC3_DIRLIGHT_COL:
				((float*)(global_params[type]))[2] = ((float*)value)[2];
				((float*)(global_params[type]))[1] = ((float*)value)[1];
					//FALLTHROUGH for the first index
			//One float
			case GLOBAL_PARAM_FLOAT_TIME:
				((float*)(global_params[type]))[0] = ((float*)value)[0];
		}
	}

	//TODO: call this from engine (probably from display init)
	//Allocates memory to hold global param values
	static void initialize_global_params()
	{
		global_params = (void**) malloc(sizeof(void*) * GLOBAL_PARAM_COUNT);

		for(int i = 0; i < GLOBAL_PARAM_COUNT; i++)
		{
			global_params[i] = NULL;
		}

		//size_t matrix_size = 16;
		size_t vector_size = 3;
		global_params[GLOBAL_PARAM_FLOAT_TIME] = malloc(sizeof(float));
		global_params[GLOBAL_PARAM_VEC3_CAM_POS] = malloc(sizeof(float)*vector_size);
		global_params[GLOBAL_PARAM_VEC3_CAM_DIR] = malloc(sizeof(float)*vector_size);
		global_params[GLOBAL_PARAM_VEC3_DIRLIGHT_DIR] = malloc(sizeof(float)*vector_size);
		global_params[GLOBAL_PARAM_VEC3_DIRLIGHT_COL] = malloc(sizeof(float)*vector_size);
		//global_params[GLOBAL_PARAM_MATRIX_VP] = malloc(sizeof(float)*matrix_size);
		//global_params[GLOBAL_PARAM_MATRIX_V] = malloc(sizeof(float)*matrix_size);
		//global_params[GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF] = malloc(sizeof(float)*matrix_size);
		//global_params[GLOBAL_PARAM_MATRIX_P_PERSP] = malloc(sizeof(float)*matrix_size);
		//global_params[GLOBAL_PARAM_MATRIX_P_ORTHO] = malloc(sizeof(float)*matrix_size);
		//global_params[GLOBAL_PARAM_MATRIX_P_INF] = malloc(sizeof(float)*matrix_size);


		//Initializing to all 0s
		memset(global_params[GLOBAL_PARAM_FLOAT_TIME], 0, sizeof(float));
		memset(global_params[GLOBAL_PARAM_VEC3_CAM_POS], 0, sizeof(float)*vector_size);
		memset(global_params[GLOBAL_PARAM_VEC3_CAM_DIR], 0, sizeof(float)*vector_size);
		memset(global_params[GLOBAL_PARAM_VEC3_DIRLIGHT_DIR], 0, sizeof(float)*vector_size);
		memset(global_params[GLOBAL_PARAM_VEC3_DIRLIGHT_COL], 0, sizeof(float)*vector_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_VP], 0, sizeof(float)*matrix_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_V], 0, sizeof(float)*matrix_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_V_NOTRANS_P_INF], 0, sizeof(float)*matrix_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_P_PERSP], 0, sizeof(float)*matrix_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_P_ORTHO], 0, sizeof(float)*matrix_size);
		//memset(global_params[GLOBAL_PARAM_MATRIX_P_INF], 0, sizeof(float)*matrix_size);
	}
	//TODO: call this from engine as well (in whatever location corresponds to the init portion)
	//Frees the memory for holding global param values
	static void free_global_params()
	{
		if(!global_params)
			return;
		for(int i = 0; i < GLOBAL_PARAM_COUNT; i++)
		{
			if(!global_params[i])
				continue;
			free(global_params[i]);
		}
		free(global_params);
	}


};
#endif //ENGINE_SHADER_H

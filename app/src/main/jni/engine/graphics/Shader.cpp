//
// Created by F1 on 5/16/2016.
//

#include "Shader.hpp"
#include "../File_Utils.hpp"
#include "Texture.hpp"
#include "Cube_Map.hpp"

//Loads the raw shader source
int Shader::load(const char* vshader_name, const char* fshader_name)
{
	vert_shader_name = (char*) malloc(sizeof(char) * (strlen(vshader_name)+1));
	frag_shader_name = (char*) malloc(sizeof(char) * (strlen(fshader_name)+1));
	strcpy(vert_shader_name,vshader_name);
	strcpy(frag_shader_name,fshader_name);

	//Loading the raw shader sources
	vert_shader_src = File_Utils::load_raw_asset(vshader_name);
	frag_shader_src = File_Utils::load_raw_asset(fshader_name);
}

//Frees the loaded raw shader source
void Shader::unload()
{
	if(vert_shader_name)
		free(vert_shader_name);
	if(frag_shader_name)
		free(frag_shader_name);
	if(vert_shader_src)
		free((void*)vert_shader_src);
	if(frag_shader_src)
		free((void*)frag_shader_src);
}

Shader::Shader(const char* vshader_name, const char* fshader_name)
{
	load(vshader_name,fshader_name);
}

Shader::~Shader()
{
	unload();
}

int Shader::init_gl (GLuint *param_types, const char **param_identifiers, uint params_count)
{
	//Creating the gl program
	gl_program = glCreateProgram();
	if(!gl_program)
	{
		LOGE("Error: failed to create gl program.\n");
		return 0;
	}

	vert_shader = GL_Utils::load_shader(vert_shader_src, vert_shader_name, GL_VERTEX_SHADER);
	glAttachShader(gl_program, vert_shader);
	frag_shader = GL_Utils::load_shader(frag_shader_src, frag_shader_name, GL_FRAGMENT_SHADER);
	glAttachShader(gl_program, frag_shader);
	glLinkProgram(gl_program);

	GLint linked;
	glGetProgramiv(gl_program, GL_LINK_STATUS, &linked);

	if(linked == GL_FALSE)
	{
		LOGE("Error: Failed to link gl program.\n");

		GLint log_size = 0;
		glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &log_size);

		GLchar *info_log = (GLchar *) malloc(sizeof(GLchar) * log_size);
		glGetProgramInfoLog(gl_program, log_size, &log_size, info_log);
		LOGE("   linker log: %s.\n", info_log);
		free(info_log);

		if(vert_shader)
			GL_Utils::unload_shader(vert_shader);
		if(frag_shader)
			GL_Utils::unload_shader(frag_shader);
		glDeleteProgram(gl_program);
		gl_program = 0;
		return 0;
	}
	glUseProgram(gl_program);


	//Create arrays with room for param_count entries
	param_type = (GLuint *) malloc(sizeof(GLuint) * params_count);
	param_location = (void **) malloc(sizeof(void *) * params_count);
	param_count = params_count;

	for(int i = 0; i < param_count; i++)
	{
		param_type[i] = param_types[i];
		param_location[i] = NULL;
		switch(param_types[i])
		{
			//Attributes
			case PARAM_VERTICES:
			case PARAM_VERT_NORMALS:
			case PARAM_VERT_TANGENTS:
			case PARAM_VERT_BINORMALS:
			case PARAM_VERT_COLORS:
			case PARAM_VERT_UV1:
			case PARAM_VERT_UV2:
			case PARAM_BONE_WEIGHTS:
			case PARAM_BONE_INDICES:
				param_location[i] = (GLint*)malloc(sizeof(GLint));
				*((GLint*)(param_location[i])) = -1;
				*((GLint*)(param_location[i])) = glGetAttribLocation(gl_program, param_identifiers[i]);
				break;
			//Uniforms
			case PARAM_CUBE_MAP:
			case PARAM_MVP_MATRIX:
			case PARAM_M_IT_MATRIX:
			case PARAM_TEXTURE_DIFFUSE:
			case PARAM_TEXTURE_NORMAL:
			case PARAM_TEXTURE_MISC:
			case PARAM_TEXTURE_LIGHTMAP:
			case PARAM_COLOR_MULT:
			case PARAM_COLOR_ADD:
			case PARAM_TEST_FIELD:
				param_location[i] = (GLint*)malloc(sizeof(GLint));
				*((GLint*)(param_location[i])) = -1;
				*((GLint*)(param_location[i])) = glGetUniformLocation(gl_program, param_identifiers[i]);
				break;
			//Matrix Arrays
			case PARAM_BONE_MATRICES:
			case PARAM_BONE_IT_MATRICES:
			{
				int matrix_count = 0;
				//we need to know how many bones the shader supports
				// keep trying to get locations for subsequent matrices until we get -1 as the address
				for(int j = 0; j < 512; j++)
				{
					char* name = NULL;
					//Format the matrix index name, i.e. if param_identifiers[i] is "bone_matrix", this yields "bone_matrix[j]", for all values of j
					if(asprintf(&name, "%s[%d]",param_identifiers[i],j) == -1)
					{
						LOGE("asprintf returns -1, failed to alloc char array for bone matrices: %s\n",param_identifiers[i]);
						continue;
					}
					if(glGetUniformLocation(gl_program, name) == -1)
					{
						free(name);
						break;
					}
					else
					{
						matrix_count++;
						free(name);
					}
				}

				//Check how many bone matrices there are.
				GLint* matrix_list_indices = (GLint*) malloc(sizeof(GLint) * matrix_count);
				for(int j = 0; j < matrix_count; j++)
				{
					char* name = NULL;
					//Format the matrix index name, i.e. if param_identifiers[i] is "bone_matrix", this yields "bone_matrix[j]", for all values of j
					if(asprintf(&name, "%s[%d]",param_identifiers[i],j) == -1)
					{
						LOGE("asprintf returns -1, failed to alloc char array for bone matrices: %s\n",param_identifiers[i]);
						continue;
					}
					matrix_list_indices[j] = glGetUniformLocation(gl_program, name);
					free(name);
				}
				param_location[i] = matrix_list_indices;
				break;
			}
			default:
				break;
		}
	}
	init_global_params();
	return 0;
}

void Shader::term_gl ()
{
	term_global_params();
	if(frag_shader)
		GL_Utils::unload_shader(frag_shader);
	if(vert_shader)
		GL_Utils::unload_shader(vert_shader);

	gl_program = 0;
	frag_shader = 0;
	vert_shader = 0;

	if(param_location)
	{
		//Freeing all allocated values in the pointer array
		for(int i = 0; i < param_count; i++)
		{
			if(param_location[i])
				free(param_location[i]);
		}
		free(param_location);
	}
	if(param_type)
	{
		free(param_type);
	}
	param_count = 0;
	glDeleteProgram(gl_program);
}

int Shader::bind_shader ()
{
	glUseProgram(gl_program);
	bound_textures = 0;
	bind_used_global_params();
	return 1;
}

//Binds a value to a shader location for rendering
int Shader::bind_shader_value (GLuint type, void *data, int extra_data)
{
	//Iterate through all parameters until we find the one we're looking for
	for(int i = 0; i < param_count; i++)
	{
		if(param_type[i] == type)
		{
			bind_shader_value_by_index(i,data,extra_data);
			break;
		}
	}
	return 1;
}


//Binds a value to a shader location for rendering, given we already know the index of the data
int Shader::bind_shader_value_by_index (int index, void *data, int extra_data)
{
	if(*((GLint*)(param_location[index])) == -1)
	{
		LOGW("Warning: shader \"%s\" param location at index %d has not been set (type: %d)",vert_shader_name,index,param_type[index]);
		return 0;
	}
	GLint loc = 0;
	GLuint uloc = 0;
	switch(param_type[index])
	{
		case PARAM_VERTICES:
		case PARAM_VERT_NORMALS:
		case PARAM_VERT_TANGENTS:
		case PARAM_VERT_BINORMALS:
			uloc = *((GLuint*)(param_location[index]));
			glVertexAttribPointer(uloc, 3, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(uloc);
			break;
		case PARAM_VERT_COLORS:
			uloc = *((GLuint*)(param_location[index]));
			glVertexAttribPointer(uloc, 4, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(uloc);
			break;
		case PARAM_VERT_UV1:
		case PARAM_VERT_UV2:
			uloc = *((GLuint*)(param_location[index]));
			glVertexAttribPointer(uloc, 2, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(uloc);
			break;
		case PARAM_M_IT_MATRIX:
			loc = *((GLint*)(param_location[index]));
			glUniformMatrix3fv(loc, 1, GL_FALSE, ((float *) data));
			break;
		case PARAM_MVP_MATRIX:
			loc = *((GLint*)(param_location[index]));
			glUniformMatrix4fv(loc, 1, GL_FALSE, ((float *) data));
			break;
		//Textures special case: passed data is a pointer to a texture object
		case PARAM_TEXTURE_DIFFUSE:
		case PARAM_TEXTURE_NORMAL:
		case PARAM_TEXTURE_LIGHTMAP:
		case PARAM_TEXTURE_MISC:
			loc = *((GLint*)(param_location[index]));
			glActiveTexture(GL_Utils::tex_index_to_enum(bound_textures));
			glBindTexture(GL_TEXTURE_2D, ((Texture*)data)->gl_id);
			glUniform1i(loc, bound_textures);
			bound_textures++;
			break;
		//Cube map special case: passed data is a pointer to a cube map object
		case PARAM_CUBE_MAP:
			loc = *((GLint*)(param_location[index]));
			glActiveTexture(GL_Utils::tex_index_to_enum(bound_textures));
			glBindTexture(GL_TEXTURE_CUBE_MAP, ((Cube_Map*)data)->gl_id);
			glUniform1i(loc, bound_textures);
			bound_textures++;
			break;
		case PARAM_BONE_MATRICES:
		{
			//extra data holds the matrix count
			//for(int i = 0; i < extra_data; i++)
			//{
			//	glUniformMatrix4fv( (((GLint*)param_location[index])[i]), 1, GL_FALSE, ((float*) data)+(16*i));
			//}

			//In the case that all bone matrices are contiguous and sequential in memory, we can just use the line below
			//This line just placed all matrices starting at the location of the first matrix
			glUniformMatrix4fv( (((GLint*)param_location[index])[0]), extra_data, GL_FALSE, ((float*) data));
			break;
		}
		case PARAM_BONE_IT_MATRICES:
		{
			//for(int i = 0; i < extra_data; i++)
			//{
			//	glUniformMatrix3fv( (((GLint*)param_location[index])[i]), 1, GL_FALSE, ((float*) data)+(9*i));
			//}
			glUniformMatrix3fv( (((GLint*)param_location[index])[0]), extra_data, GL_FALSE, ((float*) data));
			break;
		}
		case PARAM_BONE_WEIGHTS:
			uloc = *((GLuint*)(param_location[index]));
			glVertexAttribPointer(uloc, 3, GL_FLOAT, GL_FALSE, 0, (float*) data);
			glEnableVertexAttribArray(uloc);
			break;
		case PARAM_BONE_INDICES:
			uloc = *((GLuint*)(param_location[index]));
			glVertexAttribPointer(uloc, 3, GL_FLOAT, GL_FALSE, 0, (float*) data);
			glEnableVertexAttribArray(uloc);
			break;
		case PARAM_COLOR_MULT:
		case PARAM_COLOR_ADD:
		case PARAM_TEST_FIELD:
			loc = *((GLint*)(param_location[index]));
			glUniform4f(loc, ((float*)data)[0],((float*)data)[1],((float*)data)[2],((float*)data)[3]);
			break;
		default:
			break;
	}
	return 1;
}

//Global parameter SHADER identifiers
const char *Shader::GLOBAL_PARAM_FLOAT_TIME_ID = "time";
const char *Shader::GLOBAL_PARAM_VEC3_CAM_POS_ID = "cam_pos";
const char *Shader::GLOBAL_PARAM_VEC3_CAM_DIR_ID = "cam_dir";
const char *Shader::GLOBAL_PARAM_VEC3_DIRLIGHT_DIR_ID = "dirlight_dir";
const char *Shader::GLOBAL_PARAM_VEC3_DIRLIGHT_COL_ID = "dirlight_col";

//List of shader identifiers for the global parameters
const char *Shader::GLOBAL_PARAM_IDS[] =
{
	Shader::GLOBAL_PARAM_FLOAT_TIME_ID,
	Shader::GLOBAL_PARAM_VEC3_CAM_POS_ID,
	Shader::GLOBAL_PARAM_VEC3_CAM_DIR_ID,
	Shader::GLOBAL_PARAM_VEC3_DIRLIGHT_DIR_ID,
	Shader::GLOBAL_PARAM_VEC3_DIRLIGHT_COL_ID
};

//Memory for the static parameters
float Shader::global_param_float_time[1] = {0};
float Shader::global_param_vec3_cam_pos[3] = {0,0,0};
float Shader::global_param_vec3_cam_dir[3] = {0,0,0};
float Shader::global_param_vec3_dirlight_dir[3] = {0,0,0};
float Shader::global_param_vec3_dirlight_col[3] = {0,0,0};

//Array holding all global parameter locations
float *Shader::global_params[] =
{
	Shader::global_param_float_time,
	Shader::global_param_vec3_cam_pos,
	Shader::global_param_vec3_cam_dir,
	Shader::global_param_vec3_dirlight_dir,
	Shader::global_param_vec3_dirlight_col
};


void Shader::init_global_params()
{
	for(int i = 0; i < GLOBAL_PARAM_COUNT; i++)
	{
		global_param_loc[i] = -1;
		global_param_loc[i] = glGetUniformLocation(gl_program, GLOBAL_PARAM_IDS[i]);
	}
}
void Shader::term_global_params()
{
	for(int i = 0; i < GLOBAL_PARAM_COUNT; i++)
	{
		global_param_loc[i] = -1;
	}
}
//TODO: a method for unbinding used global params?
//this would be required because we search on init_gl, so we should destroy on term_gl
//Binds any of the global params that this shader uses
void Shader::bind_used_global_params()
{
	GLint loc;
	for(int type = 0; type < GLOBAL_PARAM_COUNT; type++)
	{
		if(global_param_loc[type] == -1)
			continue;

		loc = global_param_loc[type];

		switch(type)
		{
			//3 floats
			case GLOBAL_PARAM_VEC3_CAM_POS:
			case GLOBAL_PARAM_VEC3_CAM_DIR:
			case GLOBAL_PARAM_VEC3_DIRLIGHT_DIR:
			case GLOBAL_PARAM_VEC3_DIRLIGHT_COL:
				glUniform3f(loc, global_params[type][0], global_params[type][1], global_params[type][2]);
				break;
				//One float
			case GLOBAL_PARAM_FLOAT_TIME:
				glUniform1f(loc,global_params[type][0]);
				break;
			default:
				break;
		}
	}
}

//Sets static global params to be accessed by all shaders
//type is the global param to set
//value is a pointer to 1,3, or 16 float values
void Shader::set_static_global_param(int type,float *value)
{
	switch(type)
	{
		//3 floats
		case GLOBAL_PARAM_VEC3_CAM_POS:
		case GLOBAL_PARAM_VEC3_CAM_DIR:
		case GLOBAL_PARAM_VEC3_DIRLIGHT_DIR:
		case GLOBAL_PARAM_VEC3_DIRLIGHT_COL:
			global_params[type][2] = value[2];
			global_params[type][1] = value[1];
			//FALLTHROUGH for the first index
			//One float
		case GLOBAL_PARAM_FLOAT_TIME:
			global_params[type][0] = value[0];
		default:
			break;
	}
}
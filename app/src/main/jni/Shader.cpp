//
// Created by F1 on 5/16/2016.
//

#include "Shader.h"


int Shader::initialize (const char *vshader_src, const char *vshader_name, const char *fshader_src, const char *fshader_name,
	GLuint *param_types, const char **param_identifiers, uint params_count)
{
	//Creating the gl program
	gl_program = glCreateProgram();
	if(!gl_program)
	{
		LOGE("Error: failed to create gl program.\n");
		return 0;
	}

	vert_shader = GL_Utils::load_shader(vshader_src, vshader_name, GL_VERTEX_SHADER);
	glAttachShader(gl_program, vert_shader);
	frag_shader = GL_Utils::load_shader(fshader_src, fshader_name, GL_FRAGMENT_SHADER);
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
	param_type = (GLuint *) malloc(sizeof(GLuint *) * params_count);
	param_location = (GLint *) malloc(sizeof(GLint *) * params_count);
	param_count = params_count;

	for(int i = 0; i < param_count; i++)
	{
		param_location[i] = -1;
	}

	for(int i = 0; i < param_count; i++)
	{
		param_type[i] = param_types[i];
		switch(param_types[i])
		{
			case PARAM_VERTICES:
			case PARAM_VERT_COLORS:
			case PARAM_VERT_UV1:
				param_location[i] = glGetAttribLocation(gl_program, param_identifiers[i]);
				break;
			case PARAM_MVP_MATRIX:
			case PARAM_TEXTURE_DIFFUSE:
			case PARAM_TEST_FIELD:
				param_location[i] = glGetUniformLocation(gl_program, param_identifiers[i]);
				break;
			default:
				break;
		}
	}
	return 0;
}

void Shader::term ()
{
	glDeleteProgram(gl_program);
	if(frag_shader)
		GL_Utils::unload_shader(frag_shader);
	if(vert_shader)
		GL_Utils::unload_shader(vert_shader);

	gl_program = 0;
	frag_shader = 0;
	vert_shader = 0;

	if(param_location)
		free(param_location);
	if(param_type)
		free(param_type);
	param_count = 0;
}

int Shader::bind_shader ()
{
	glUseProgram(gl_program);
	bound_textures = 0;
	return 1;
}

//Binds a value to a shader location for rendering
int Shader::bind_shader_value (GLuint type, void *data)
{
	//Iterate through all parameters until we find the one we're looking for
	for(int i = 0; i < param_count; i++)
	{
		if(param_type[i] == type)
		{
			bind_shader_value_by_index(i,data);
			break;
		}
	}
	return 1;
}

//Binds a value to a shader location for rendering, given we already know the index of the data
int Shader::bind_shader_value_by_index (int index, void *data)
{
	GLuint loc;//used for vertex attributes that require the locations to be unsigned ints
	if(param_location[index] == -1)
	{
		LOGE("Warning: param location at index %d has not been set",index);
		return 0;
	}
	loc = 0;
	//Handle parameter locations that expect unsigned int values
	switch(param_type[index])
	{
		case PARAM_VERTICES:
		case PARAM_VERT_COLORS:
		case PARAM_VERT_UV1:
			if(param_location[index] > INT_MAX)
			{
				LOGE("Error: unsigned int parameter location in shader is greater than the capacity of int.\n");
				return 0;
			}
			loc = (GLuint) param_location[index];
			break;
		default:
			break;
	}

	switch(param_type[index])
	{
		case PARAM_VERTICES:
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(loc);
			break;
		case PARAM_VERT_COLORS:
			glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(loc);
			break;
		case PARAM_VERT_UV1:
			glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (float *) data);
			glEnableVertexAttribArray(loc);
			break;
		case PARAM_MVP_MATRIX:
			glUniformMatrix4fv(param_location[index], 1, GL_FALSE, ((float *) data));
			break;
		case PARAM_TEXTURE_DIFFUSE:
			glActiveTexture(GL_Utils::tex_index_to_enum(bound_textures));
			glBindTexture(GL_TEXTURE_2D, (GLuint) data);
			glUniform1i(param_location[index], bound_textures);
			bound_textures++;
			break;
		case PARAM_BONE_MATRICES:
			//glUniformMatrix4fv(param_location[index], (how do we get the count?), GL_FALSE, (??? have to convert a mat4 array to just an array of arrays of floats);
			break;
		case PARAM_BONE_WEIGHTS:
		case PARAM_BONE_INDICES:
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (float*) data);
			break;
		case PARAM_TEST_FIELD:
			glUniform4f(param_location[index], ((float*)data)[0],((float*)data)[1],((float*)data)[2],((float*)data)[3]);
			break;
		default:
			break;
	}
	return 1;
}

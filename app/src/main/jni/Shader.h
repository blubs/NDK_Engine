//
// Created by F1 on 5/16/2016.
//
#include "common.h"
#include "GL_Utils.h"

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H


//FIXME: all of this code is placeholder code that will be replaced as I figure out the implementation details
class Shader
{
public:
	//we construct a shader by passing in the shader source files
	GLuint gl_program = 0;

	GLuint vert_shader;
	GLuint frag_shader;


	//TODO: modularize this a bit (how do we arbitrarily specify what attributes a shader needs?)

	//Static types for parameters
	static const int PARAM_VERTICES = 1;
	//static const int PARAM_NORMALS = 2; TODO: later.
	static const int PARAM_VERT_COLORS = 3;
	static const int PARAM_VERT_UV1 = 4;
	//static const int PARAM_VERT_UV2 = 5; TODO: later
	static const int PARAM_MVP_MATRIX = 6;
	static const int PARAM_TEXTURE = 7;//this one can be left vague, because whatever the identifier, it just goes there & shouldn't change.
	//TODO: skeleton list of matrices
	//Any other parameter types I can think of?

	//Arrays that hold arbitrary types of parameters
	GLint* param_location;
	GLuint* param_type;
	int param_count;


	//has a function for initializing a shader (accepts vert & frag shader source code)
	//int initialize(char* vert_shader_src,char* vert_shader_name,char* frag_shader_src,char* frag_shader_name)
	int initialize(const char* vshader_src, const char* vshader_name, const char* fshader_src, const char* fshader_name,
				GLuint* param_types, const char** param_identifiers, int params_count)
	{
		//Creating the gl program
		gl_program = glCreateProgram();
		if(!gl_program)
		{
			LOGE("Error: failed to create gl program.\n");
			return 0;
		}

		vert_shader = GL_Utils::load_shader(vshader_src,vshader_name,GL_VERTEX_SHADER);
		glAttachShader(gl_program, vert_shader);
		frag_shader = GL_Utils::load_shader(fshader_src,fshader_name,GL_FRAGMENT_SHADER);
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
		param_type = (GLuint*) malloc(sizeof(GLuint*) * params_count);
		param_location = (GLint*) malloc(sizeof(GLint*) * params_count);
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
				case PARAM_TEXTURE:
					param_location[i] = glGetUniformLocation(gl_program, param_identifiers[i]);
					break;
				default:
					break;
			}
		}
		return 0;
	}

	void term()
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

	int bind_shader()
	{
		glUseProgram(gl_program);
	}
	//Accepted an array of all of the required data? or some other method of binding the data?
	int bind_shader_value(GLuint type,void* data)
	{
		int used_textures = 0;
		//Iterate through all parameters until we find the one we're looking for
		for(int i = 0; i < param_count; i++)
		{
			if(param_type[i] == type)
			{
				if(param_location[i] == -1)
					continue;

				switch(type)
				{
					case PARAM_VERTICES:
						glVertexAttribPointer(param_location[i], 3, GL_FLOAT, GL_FALSE, 0, (float *) data);
						glEnableVertexAttribArray(param_location[i]);
						break;
					case PARAM_VERT_COLORS:
						glVertexAttribPointer(param_location[i], 4, GL_FLOAT, GL_FALSE, 0, (float*) data);
						glEnableVertexAttribArray(param_location[i]);
						break;
					case PARAM_VERT_UV1:
						glVertexAttribPointer(param_location[i],2, GL_FLOAT, GL_FALSE, 0, (float*) data);
						glEnableVertexAttribArray(param_location[i]);
						break;
					case PARAM_MVP_MATRIX:
						glUniformMatrix4fv(param_location[i],1,GL_FALSE,((Mat4*)data)->m);
						break;
					case PARAM_TEXTURE:
						//FIXME: this will not work with more than one texture
						//	need to do a switch case to get appropriate texture slot
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, (GLuint) data);
						glUniform1i(param_location[i], used_textures);
						used_textures++;
						break;
					default:
						break;
				}
				break;
			}
		}
		//need to somehow get the actual values of the parameters, recognize what is what using type:
		//then bind them using functions

	}
};
#endif //ENGINE_SHADER_H

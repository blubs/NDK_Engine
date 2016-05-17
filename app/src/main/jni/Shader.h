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
	//We only use these structs to pass pointers, so dealloc these structs (but not their pointed data) immediately after creating the shader
	struct shader_data
	{
		const char* name;
		const char* src;
	};

	struct texture_data
	{
		const char* name;
		const char* identifier;
		int file_size;
		const char* data;
		int resolution;
	};

		//we construct a shader by passing in the shader source files
	GLuint gl_program = 0;

	GLuint vert_shader;
	GLuint frag_shader;

	//TODO: modularize this a bit (how do we arbitrarily specify what attributes a shader needs?)

	//Static types for parameters
	static const int PARAM_VERT_SHADER = 1;
	static const int PARAM_FRAG_SHADER = 2;
	//static const int PARAM_VERT_ATTRIBUTE = 3; these 3 are too vague, replace with purpose-defining parameter types
	//static const int PARAM_UNIFORM = 4;
	//static const int PARAM_TEXTURE = 5;
	static const int PARAM_VERTICES = 3;
	//static const int PARAM_NORMALS = 4; TODO: later.
	static const int PARAM_VERT_COLORS = 5;
	static const int PARAM_VERT_UV1 = 6;
	//static const int PARAM_VERT_UV2 = 7; TODO: later
	static const int PARAM_MVP_MATRIX = 8;
	static const int PARAM_TEXTURE = 9;//this one can be left vague, because whatever the identifier, it just goes there & shouldn't change.
	//TODO: skeleton list of matrices
	//Any other parameter types I can think of?

	//FIXME: how do we distinguish the fields?
	//i.e. how does the binding function know where to place the vertex buffer, the uv buffer, the vert color buffer, and the matrix buffer...?
	//we have to have some sort of standardized language of where things go...
	//but if we know where things go, then we know what parameter type they are, so I'm not sure the above parameter type defs are required..

	//Explicit shader value definitions
/*
	GLint shader_vert_pos_loc = -1;
	GLint shader_fill_color_loc = -1;
	GLint shader_tex_loc = -1;
	GLint shader_uv_loc = -1;
	GLint shader_mvp_loc = -1;

	GLuint test_tex = 0;
	GLuint texture_id = 0;
 */

	//Test handling of multiple parameters...
	//If we want to handle arbitrary parameter types and values, we can't explicitly specify parameters. (as the code above and what we did before)
	GLuint param_location[];
	GLuint param_type[];
	GLuint param_data[];
	//problem: certain parameters require more than one piece of information...
	//i.e. images require at least the data, name of the texture in the shader, resolution, & file size, unless I pass in an image struct that has all of this stuff
	//we pass this one into the initializer
	const char* param_name[];


	//has a function for initializing a shader (accepts vert & frag shader source code)
	//int initialize(char* vert_shader_src,char* vert_shader_name,char* frag_shader_src,char* frag_shader_name)
	int initialize(int* param_types, int* params, int param_count)
	{
		//Creating the gl program
		gl_program = glCreateProgram();
		if(!gl_program)
		{
			LOGE("Error: failed to create gl program.\n");
			return 0;
		}

		for(int i = 0; i < param_count; i++)
		{
			switch(param_types[i])
			{
				case PARAM_VERT_SHADER:
					break;
				case PARAM_FRAG_SHADER:
					break;
				case PARAM_VERTICES:
					break;
				case PARAM_VERT_COLORS:
					break;
				case PARAM_VERT_UV1:
					break;
				case PARAM_MVP_MATRIX:
					break;
				case PARAM_TEXTURE:
					break;
				default:
					break;
			}
		}

		//Compiling the shaders
		vert_shader = GL_Utils::load_shader(vert_shader_src,vert_shader_name,GL_VERTEX_SHADER);
		frag_shader = GL_Utils::load_shader(frag_shader_src,frag_shader_name,GL_FRAGMENT_SHADER);
		glAttachShader(gl_program, vert_shader);
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

			glDeleteProgram(gl_program);
			gl_program = 0;
			return 0;
		}
		glUseProgram(gl_program);

		shader_fill_color_loc = glGetAttribLocation(gl_program, "fill_color");
		shader_vert_pos_loc = glGetAttribLocation(gl_program, "vert_pos");
		shader_uv_loc = glGetAttribLocation(gl_program, "src_tex_coord");
		shader_tex_loc = glGetUniformLocation(gl_program, "tex");
		shader_mvp_loc = glGetUniformLocation(gl_program,"mvp");


		//==================================== Loading texture ===============================
		GLuint tex_id;
		glGenTextures(1, &tex_id);

		//how do we pass in the texture?
		//I wonder if we should just hold an array of structs
		//======================
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC1_RGB8, 512, 512, 0, 131072, (const void *) test_tex/**/);//need a way to pass in shader parameters
		glBindTexture(GL_TEXTURE_2D,0);

		texture_id = tex_id;
	}

	void term()
	{
		glDeleteTextures(1, &texture_id);
		glDeleteProgram(gl_program);
		if(frag_shader)
			GL_Utils::unload_shader(frag_shader);
		if(vert_shader)
			GL_Utils::unload_shader(vert_shader);

		gl_program = 0;
		shader_vert_pos_loc = -1;
		shader_fill_color_loc = -1;
		shader_uv_loc = -1;
		shader_tex_loc = -1;
		shader_mvp_loc = -1;
		texture_id = 0;
		frag_shader = 0;
		vert_shader = 0;
	}
	//has a function for binding a shader, accepting any required vertex data, and matrix data
	//has a function for terminating a shader
};
#endif //ENGINE_SHADER_H

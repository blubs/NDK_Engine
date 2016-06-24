//
// Created by F1 on 5/16/2016.
//
#include "common.h"
#include "GL_Utils.h"

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H


class Shader
{
public:
	//Holding the raw src code of the shaders
	const char* vert_shader_src;
	const char* frag_shader_src;
	char* vert_shader_name;
	char* frag_shader_name;

	//we construct a shader by passing in the shader source files
	GLuint gl_program = 0;

	GLuint vert_shader;
	GLuint frag_shader;

	//Static types for parameters
	static const int PARAM_VERTICES = 1;
	static const int PARAM_VERT_NORMALS = 2;
	static const int PARAM_VERT_COLORS = 3;
	static const int PARAM_VERT_UV1 = 4;
	//static const int PARAM_VERT_UV2 = 5; TODO: later
	static const int PARAM_MVP_MATRIX = 6;
	static const int PARAM_TEXTURE_DIFFUSE = 7;
	//static const int PARAM_TEXTURE_NORMAL = 8; TODO: later
	//static const int PARAM_TEXTURE_MISC = 9; TODO: later
	//static const int PARAM_TEXTURE_LIGHTMAP = 10; TODO: later
	static const int PARAM_BONE_MATRICES = 11;
	static const int PARAM_BONE_INDICES = 12;
	static const int PARAM_BONE_WEIGHTS = 13;
	static const int PARAM_COLOR_MULT = 14;
	static const int PARAM_COLOR_ADD = 15;
	static const int PARAM_TEST_FIELD = 20;
	//TODO: do cubemap textures need their own distinct texture types for each face?
	//TODO: any other parameter types

	//Arrays that hold arbitrary types of parameters
	void** param_location;
	GLuint* param_type;
	uint param_count;

	//Running count of how many textures we have bound. Resets every time bind is initiated.
	int bound_textures;

	//Loads the raw shader source
	int load(const char* vshader_name, const char* fshader_name);

	//Frees the loaded raw shader source
	void unload();

	//Initializes the shader given vertex / fragment shader src and name, as well as a list of parameter types and values (including the number of parameters)
	int init_gl(GLuint* param_types, const char** param_identifiers, uint params_count);

	//Freeing the shader and allocated memory
	void term_gl();
	//Binds a shader for use by renderer (should be called before binding individual values)
	int bind_shader();

	//Binds a value to a shader location for rendering
	int bind_shader_value(GLuint type,void* data, int extra_data);

	//Same as above if we already know what index the data goes in.
	int bind_shader_value_by_index (int index, void *data, int extra_data);

};
#endif //ENGINE_SHADER_H

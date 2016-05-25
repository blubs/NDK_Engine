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
	//we construct a shader by passing in the shader source files
	GLuint gl_program = 0;

	GLuint vert_shader;
	GLuint frag_shader;

	//Static types for parameters
	static const int PARAM_VERTICES = 1;
	//static const int PARAM_NORMALS = 2; TODO: later
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
	static const int PARAM_TEST_FIELD = 20;
	//TODO: do cubemap textures need their own distinct texture types for each face?
	//TODO: any other parameter types

	//Arrays that hold arbitrary types of parameters
	void** param_location;
	GLuint* param_type;
	uint param_count;

	//Running count of how many textures we have bound. Resets every time bind is initiated.
	int bound_textures;

	//Initializes the shader given vertex / fragment shader src and name, as well as a list of parameter types and values (including the number of parameters)
	int initialize(const char* vshader_src, const char* vshader_name, const char* fshader_src, const char* fshader_name,
				GLuint* param_types, const char** param_identifiers, uint params_count);

	//Freeing the shader and allocated memory
	void term();
	//Binds a shader for use by renderer (should be called before binding individual values)
	int bind_shader();

	//Binds a value to a shader location for rendering
	int bind_shader_value(GLuint type,void* data);

	//Same as above if we already know what index the data goes in.
	int bind_shader_value_by_index (int index, void *data);

};
#endif //ENGINE_SHADER_H

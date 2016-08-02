//
// Created by F1 on 5/16/2016.
//
#include "../common.hpp"
#include "Shader.hpp"

#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H

class Material
{
public:
	//We need to keep track of all of the fixed parameters and all of the varying parameters
	Material();
	~Material();

	Shader* shader;
	//has all parameters of shader (textures, color values, other parameters)
	void** alloced_params;//parameters we have allocated
	void** params;//pointers to externally allocated memory
	uint param_count;

	//Assigns shader value and creates params array
	int set_shader(Shader* shad);

	//Have the material hold a shader parameter (copies and holds the data)
	int set_fixed_shader_param(GLint type, void* value, int param_length);

	//Have the material hold a shader parameter pointer (holds a pointer to the data)
	int set_fixed_shader_param_ptr(GLint type, void* value);

	//Sets material for use by renderer
	int bind_material();

	//Pass arbitrary value directly to the shader
	int bind_value(GLuint type, void* value);

	int bind_values(GLuint type, void* value, int count);

private:
	void clear_all_params();
};

#endif //ENGINE_MATERIAL_H
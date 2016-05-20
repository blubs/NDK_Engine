//
// Created by F1 on 5/16/2016.
//
#include "common.h"
#include "Shader.h"

#ifndef ENGINE_MATERIAL_H
#define ENGINE_MATERIAL_H

class Material
{
public:
	//We need to keep track of all of the fixed parameters and all of the varying parameters
	//


	Shader* shader;
	//has all parameters of shader (textures, color values, other parameters)
	void** alloced_params;//parameters we have allocated
	void** params;//pointers to externally allocated memory
	uint param_count;

	//Should only be called at startup, to clear out our fields.
	int initialize()
	{
		alloced_params = NULL;
		params = NULL;
		param_count = 0;
		shader = NULL;
	}

	//Assigns shader value and creates params array
	int set_shader(Shader* shad)
	{
		if(shader)
		{
			clear_all_params();
			free(alloced_params);
			free(params);
			param_count = 0;
		}
		shader = shad;
		param_count = shader->param_count;

		alloced_params = (void**) malloc(sizeof(void*) * param_count);
		memset(alloced_params,0, sizeof(void*) * param_count);

		params = (void**) malloc(sizeof(void*) * param_count);
		memset(params, 0, sizeof(void*) * param_count);

		return 1;
	}

	//Have the material hold a shader parameter (copies and holds the data)
	int set_fixed_shader_param(GLint type, void* value, int param_length)
	{
		if(!shader)
		{
			LOGE("Error: tried to assign a shader parameter without first assigning shader.\n");
			return 0;
		}
		//iterate through shader parameter types, and place it in our parameter array at the same index
		for(int i = 0; i < param_count; i++)
		{
			if(type == shader->param_type[i])
			{
				//Actually allocate our own memory for the pointer
				alloced_params[i] = malloc(sizeof(char) * param_length);
				memcpy((alloced_params[i]),value,(size_t)param_length);
				return 1;
			}
		}

		//If we didn't find the parameter type.
		//This isn't an error, so don't crash. Just let me know we didn't use this parameter.
		LOGE("Warning: couldn't find shader parameter type %d\n",type);
		return 1;
	}

	//Have the material hold a shader parameter pointer (holds a pointer to the data)
	int set_fixed_shader_param_ptr(GLint type, void* value)
	{
		if(!shader)
		{
			LOGE("Error: tried to assign a shader parameter without first assigning shader.\n");
			return 0;
		}
		//iterate through shader parameter types, and place it in our parameter array at the same index
		for(int i = 0; i < param_count; i++)
		{
			if(type == shader->param_type[i])
			{
				//Just copy the pointer value
				params[i] = value;
				return 1;
			}
		}

		//If we didn't find the parameter type.
		//This isn't an error, so don't crash. Just let me know we didn't use this parameter.
		LOGE("Warning: couldn't find shader parameter type %d\n",type);
		return 1;
	}

	//Sets material for use by renderer
	int bind_material()
	{
		if(!shader)
		{
			LOGE("Error: tried to use a material that has no shader\n");
			return 0;
		}
		shader->bind_shader();
		for(int i = 0; i < param_count; i++)
		{
			if(params[i])
			{
				LOGE("bind material: found param %d , binding it\n",i);
				shader->bind_shader_value_by_index(i, &(params[i]));
			}
			else if(alloced_params[i])
			{
				LOGE("bind material: found alloced param %d , binding it\n",i);
				LOGE("type = %d, TEST_PARAM = %d",shader->param_type[i], Shader::PARAM_TEST_FIELD);
				shader->bind_shader_value_by_index(i, &(alloced_params[i]));
			}
		}
		return 1;
	}

	//Pass arbitrary value directly to the shader
	int bind_value(GLuint type, void* value)
	{
		if(!shader)
		{
			LOGE("Error: tried to bind a shader value without first assigning shader.\n");
			return 0;
		}
		return shader->bind_shader_value(type, value);
	}

	//has a function for terminating this material
	void term()
	{
		LOGE("terminate material called\n");
		if(params)
		{
			LOGE("params is initialiezd\n");
			clear_all_params();
			free(params);
			free(alloced_params);
		}
		param_count = 0;
		shader = NULL;
		LOGE("terminate material finished\n");
	}

private:
	void clear_all_params()
	{
		for(int i = 0; i < param_count; i++)
		{
			if(alloced_params[i])
			{
				LOGE("found allocated param at index %d, freeing it",i);
				free(alloced_params[i]);
			}

			alloced_params[i] = NULL;
			params[i] = NULL;
		}
	}
};

#endif //ENGINE_MATERIAL_H

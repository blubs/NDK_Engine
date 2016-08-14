//
// Created by F1 on 5/16/2016.
//

#include "Material.hpp"


//Should only be called at startup, to clear out our fields.
Material::Material()
{
	alloced_params = NULL;
	params = NULL;
	param_count = 0;
	shader = NULL;
}


//has a function for terminating this material
Material::~Material()
{
	if(params)
	{
		clear_all_params();
		free(params);
		free(alloced_params);
	}
	param_count = 0;
	shader = NULL;
}

//Assigns shader value and creates params array
int Material::set_shader (Shader *shad)
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

	alloced_params = (void **) malloc(sizeof(void *) * param_count);
	memset(alloced_params, 0, sizeof(void *) * param_count);

	params = (void **) malloc(sizeof(void *) * param_count);
	memset(params, 0, sizeof(void *) * param_count);

	return 1;
}

//Have the material hold a shader parameter (copies and holds the data)
int Material::set_fixed_shader_param (GLint type, void *value, int param_length)
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
			alloced_params[i] = malloc(param_length);
			memcpy((alloced_params[i]), value, (size_t) param_length);
			return 1;
		}
	}

	//If we didn't find the parameter type.
	//This isn't an error, so don't crash. Just let me know we didn't use this parameter.
	LOGE("Warning: couldn't find shader parameter type %d\n", type);
	return 1;
}

//Have the material hold a shader parameter pointer (holds a pointer to the data)
int Material::set_fixed_shader_param_ptr (GLint type, void *value)
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
	LOGE("Warning: couldn't find shader parameter type %d\n", type);
	return 1;
}

//Sets material for use by renderer
int Material::bind_material ()
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
			shader->bind_shader_value_by_index(i, params[i], 0);
		}
		else
		{
			if(alloced_params[i])
			{
				shader->bind_shader_value_by_index(i, alloced_params[i], 0);
			}
		}
	}
	return 1;
}

//Pass arbitrary value directly to the shader
int Material::bind_value (GLuint type, void *value)
{
	if(!shader)
	{
		LOGE("Error: tried to bind a shader value without first assigning shader.\n");
		return 0;
	}

	if(type == Shader::PARAM_BONE_MATRICES)
	{
		LOGE("Error: Param bone matrices should be sent with the method \"bind_values\", not \"bind_value\"\n");
		return 0;
	}
	return shader->bind_shader_value(type, value, 0);
}

int Material::bind_values (GLuint type, void *value, int count)
{
	if(!shader)
	{
		LOGE("Error: tried to bind a shader value without first assigning shader.\n");
		return 0;
	}
	return shader->bind_shader_value(type, value, count);
}

void Material::clear_all_params ()
{
	for(int i = 0; i < param_count; i++)
	{
		if(alloced_params[i])
		{
			free(alloced_params[i]);
		}

		alloced_params[i] = NULL;
		params[i] = NULL;
	}
}

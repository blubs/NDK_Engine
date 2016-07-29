//
// Created by F1 on 7/1/2016.
//

#ifndef ENGINE_SKYBOX_H
#define ENGINE_SKYBOX_H

#include "Cube_Map.hpp"
#include "Shader.hpp"
#include "math.hpp"

class Skybox
{
public:
	Shader* shader;
	Cube_Map* cube_map;

	GLuint tri_verts_buffer;
	//Due to simplicity of skybox, we don't require a skybox material, and it can hold its own shaders
	Skybox()
	{
		shader = (Shader*) malloc(sizeof(Shader));
	}
	~Skybox()
	{
		if(shader)
		{
			free(shader);
		}
	}

	int load_shader()
	{
		if(shader)
		{
			shader->load("shaders/skybox.vert","shaders/skybox.frag");
		}
		return 1;
	}
	void unload_shader()
	{
		if(shader)
		{
			shader->unload();
		}
	}

	int init_gl()
	{
		//Setting up vertex buffer objects
		glGenBuffers(1, &tri_verts_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);

		int tri_vert_count = 36;

		unsigned int tri_verts[] =
		{
			//Front quad
			0, 2, 1, 2, 0, 3,
			//Back quad
			4, 6, 5, 6, 4, 7,
			//Right quad
			1, 7, 4, 7, 1, 2,
			//Left quad
			0, 6, 3, 6, 0, 5,
			//Top quad
			4, 0, 1, 0, 4, 5,
			//Bottom quad
			6, 2, 3, 2, 6, 7,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(tri_vert_count * sizeof(unsigned int)), tri_verts, GL_STATIC_DRAW);


		//Setting up skybox shader
		GLuint param_types[] =
		{
			Shader::PARAM_VERTICES,
			Shader::PARAM_CUBE_MAP,
			Shader::PARAM_MVP_MATRIX
		};
		const char *param_names[] =
		{
			"vert_pos",
			"cube_map",
			"mvp"
		};
		shader->init_gl(param_types, param_names,3);
		return 1;
	}
	void term_gl()
	{
		//Freeing vertex buffer objects
		glDeleteBuffers(1,&tri_verts_buffer);
		//Terminating skybox shader
		if(shader)
			shader->term_gl();
	}

	int set_cube_map(Cube_Map* map)
	{
		cube_map = map;
		return 1;
	}

	int render(Mat4 v_matrix)
	{
		if(!cube_map)
		{
			LOGW("Warning: tried rendering skybox without running assigning a cube map\n");
			return 1;
		}
		shader->bind_shader();

		shader->bind_shader_value(Shader::PARAM_MVP_MATRIX,(void*) v_matrix.m,0);
		shader->bind_shader_value(Shader::PARAM_CUBE_MAP,(void*) cube_map->gl_id,0);

		const float cube_vertices[] =
		{
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
		};
		shader->bind_shader_value(Shader::PARAM_VERTICES,(void*) cube_vertices,0);

		int tri_vert_count = 36;//36 vertices in cube (3 for every tri, 2 tris per face, 6 faces: 3*2*6=36)

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);
		glDrawElements(GL_TRIANGLES, tri_vert_count, GL_UNSIGNED_INT, (void *) 0);
	}

};

#endif //ENGINE_SKYBOX_H

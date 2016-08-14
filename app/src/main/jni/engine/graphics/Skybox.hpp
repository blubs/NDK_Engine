//
// Created by F1 on 7/1/2016.
//

#ifndef ENGINE_SKYBOX_H
#define ENGINE_SKYBOX_H

#include "Cube_Map.hpp"
#include "Shader.hpp"
#include "../math/math.hpp"

class Skybox
{
public:
	Shader* shader;
	Cube_Map* cube_map;

	GLuint tri_verts_buffer;
	//Due to simplicity of skybox, we don't require a skybox material, and it can hold its own shaders
	Skybox();
	~Skybox();

	int init_gl();
	void term_gl();
	int set_cube_map(Cube_Map* map);

	int render(Mat4 v_matrix);
};

#endif //ENGINE_SKYBOX_H

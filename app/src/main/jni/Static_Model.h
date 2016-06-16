//
// Created by F1 on 5/5/2016.
//

#ifndef ENGINE_STATIC_MODEL_H
#define ENGINE_STATIC_MODEL_H

#include "common.h"
#include "Material.h"
#include "File_Utils.h"
#include "math/math.h"
class Static_Model
{
public:
	unsigned int vertex_count;
	unsigned int tri_vert_count;


	const unsigned int* raw_data = NULL;
	const float* verts;

	const unsigned int* tri_verts;


	//arrays of vertex attributes
	//	uvs
	//	normals
	//		do we need tangents?

	GLuint tri_verts_buffer;

	int render(Mat4 mvp,Material* mat);

	//We will eventually need methods for combining Meshes, and copying Meshes
	int load_model(const char* filepath);

	int init_gl();

	void term_gl();
	void unload_model();

};

#endif //ENGINE_STATIC_MODEL_H

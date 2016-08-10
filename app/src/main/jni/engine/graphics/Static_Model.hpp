//
// Created by F1 on 5/5/2016.
//

#ifndef ENGINE_STATIC_MODEL_H
#define ENGINE_STATIC_MODEL_H

#include "../common.hpp"
#include "Material.hpp"
#include "../File_Utils.hpp"
#include "../math/math.hpp"
class Static_Model
{
public:
	unsigned int vertex_count;
	unsigned int tri_vert_count;


	const unsigned int* raw_data = NULL;
	const float* verts;
	const float* uv_coords_1;
	const float* uv_coords_2;
	const float* normals;
	const float* tangents;
	const float* binormals;

	const unsigned int* tri_verts;


	//arrays of vertex attributes
	//	uvs
	//	normals
	//		do we need tangents?

	GLuint tri_verts_buffer;

	int render(Mat4 m,Mat4 vp,Material* mat);

	//We will eventually need methods for combining Meshes, and copying Meshes
	int init_gl();

	void term_gl();

	Static_Model(const char* filepath);
	~Static_Model();
private:
	int load_model(const char* filepath);
	void unload_model();

};

#endif //ENGINE_STATIC_MODEL_H

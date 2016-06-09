//
// Created by F1 on 5/29/2016.
//

#ifndef ENGINE_SKEL_MODEL_H
#define ENGINE_SKEL_MODEL_H

#include "common.h"
#include "Material.h"
#include "File_Utils.h"
#include "Skeleton.h"

class Skel_Model
{
public:
	//Holds some transform variables
	Vec3 pos;
	Vec3 angles;

	//Holds a reference to the parent transform (for getting to object space, then iterating up to world space)
	//somevartype* parent;//I guess it can only be a child of objects, not other meshes,
	//so whatever our gameobject / entity type is, use that as parent reference.
	//unless we bind it to a bone transform directly, have to figure this one out

	//Holds a reference to skeleton (todo: Skeleton class)
	//	skeleton holds reference to animations (todo: Animation class )

	//holds all attributes
	unsigned int vertex_count;
	unsigned int tri_vert_count;


	const unsigned int* raw_data = NULL;
	const float* verts;

	const float* bone_indices;
	const float* bone_weights;

	const unsigned int* tri_verts;


	//arrays of vertex attributes
	//	uvs
	//	normals
	//		do we need tangents?

	Material* mat;

	Skeleton* skel;

	GLuint tri_verts_buffer;

	int render(Mat4 mvp);


	//Should MeshUtils be its own class, or should we just add a method here for parsing a mesh file into a Mesh structure?


	//We will eventually need methods for combining Meshes, and copying Meshes

	int load_model(const char* filepath);

	int init_gl();

	void term_gl();
	void unload_model();

};



#endif //ENGINE_SKEL_MODEL_H

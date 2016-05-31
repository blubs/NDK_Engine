//
// Created by F1 on 5/29/2016.
//

#ifndef ENGINE_SKEL_MODEL_H
#define ENGINE_SKEL_MODEL_H

#include "common.h"
#include "Material.h"
#include "File_Utils.h"


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

	const unsigned int* bone_indices;
	const float* bone_weights;

	const unsigned int* tri_verts;


	//arrays of vertex attributes
	//	uvs
	//	normals
	//		do we need tangents?

	Material* mat;

	GLuint tri_verts_buffer;

	//Has a function for rendering the model (binds all data)

	//Has a function for binding gl VBOs
	//Also has a function for unbinding gl VBOs
	int render(Mat4 vp)
	{
		Vec3 pos(0,-8.5f,-0.5f);

		Mat4 model_pos = Mat4::TRANSLATE(pos);

		static float angle = 0.0f;
		angle += 0.01f;
		if(angle > 360.0f)
			angle = 0.0f;

		Quat rot(angle,Vec3::UP());
		Mat4 model_rot = Mat4::ROTATE(rot);
		Mat4 model_transform = model_pos * model_rot;
		Mat4 mvp = vp * model_transform;


		mat->bind_material();
		mat->bind_value(Shader::PARAM_VERTICES, (void*) verts);
		mat->bind_value(Shader::PARAM_MVP_MATRIX, (void*) mvp.m);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);

		//glDrawArrays(GL_TRIANGLES, 0, vert_count);
		glDrawElements(GL_TRIANGLES, tri_vert_count, GL_UNSIGNED_INT, (void *) 0);
	}


	//Should MeshUtils be its own class, or should we just add a method here for parsing a mesh file into a Mesh structure?


	//We will eventually need methods for combining Meshes, and copying Meshes

	int load_model(const char* filename)
	{
		raw_data = (unsigned int*) File_Utils::load_raw_asset(filename);
		if(!raw_data)
		{
			LOGE("Error: failed to load \"%s\"\n",filename);
			return 0;
		}

		//File Schematics
		//First float is the vertex count
		//Second float is the triangle count * 3
		//List thereafter is the position (3 floats) of all vertices
		//List thereafter is 3 bone indices for all vertices
		//List thereafter is 3 bone weights pertaining to the 3 bone indices defined above
		//List thereafter is the indices vertices that make up the triangles
		vertex_count = raw_data[0];
		tri_vert_count = raw_data[1];

		verts = (float*) (raw_data + 2);
		bone_indices = raw_data + 2 + (vertex_count*3);
		bone_weights = (float*) raw_data + 2 + (vertex_count*3) + (vertex_count*3);
		tri_verts = raw_data + 2 + (vertex_count*3) + (vertex_count*3) + (vertex_count*3);
		return 1;
	}

	int init_gl()
	{
		glGenBuffers(1, &tri_verts_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(tri_vert_count * sizeof(unsigned int)), tri_verts, GL_STATIC_DRAW);

		return 1;
	}

	void term_gl()
	{
		glDeleteBuffers(1,&tri_verts_buffer);
	}
	//TODO: free all allocated data
	void unload_model()
	{
		if(raw_data)
			free((void*)raw_data);
	}

};



#endif //ENGINE_SKEL_MODEL_H

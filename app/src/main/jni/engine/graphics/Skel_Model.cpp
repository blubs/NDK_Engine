//
// Created by F1 on 5/29/2016.
//

#include "Skel_Model.hpp"

int Skel_Model::render(Mat4 m,Mat4 vp, Material* mat)
{
	if(!mat)
	{
		LOGW("Warning: tried rendering a skeletal model without passing a material\n");
		return 0;
	}
	if(!skel)
	{
		LOGW("Warning: tried rendering a skeletal model without assigning a skeleton\n");
		return 0;
	}

	mat->bind_value(Shader::PARAM_VERTICES, (void*) verts);
	mat->bind_value(Shader::PARAM_VERT_UV1, (void*) uv_coords);
	mat->bind_value(Shader::PARAM_VERT_NORMALS, (void*) normals);
	mat->bind_value(Shader::PARAM_VERT_TANGENTS, (void*) tangents);
	mat->bind_value(Shader::PARAM_VERT_BINORMALS, (void*) binormals);

	Mat4 mvp = vp * m;
	mat->bind_value(Shader::PARAM_MVP_MATRIX, (void*) mvp.m);

	Mat3 m_it = m.inverted_then_transposed().get_mat3();
	mat->bind_value(Shader::PARAM_M_IT_MATRIX, (void*) m_it.m);

	mat->bind_value(Shader::PARAM_BONE_INDICES, (void*) bone_indices);
	mat->bind_value(Shader::PARAM_BONE_WEIGHTS, (void*) bone_weights);


	float* pose_data = skel->get_current_pose();
	mat->bind_values(Shader::PARAM_BONE_MATRICES, (void*) (pose_data),skel->bone_count);

	float* pose_IT_data = skel->get_current_pose_IT();
	mat->bind_values(Shader::PARAM_BONE_IT_MATRICES, (void*) (pose_IT_data),skel->bone_count);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);

	glDrawElements(GL_TRIANGLES, tri_vert_count, GL_UNSIGNED_INT, (void *) 0);

	/*GLenum error = glGetError();
	while(error != GL_NO_ERROR)
	{
		LOGE("GL error: %d",error);
		error = glGetError();
	}*/

	return 1;
}

int Skel_Model::load_model(const char* filepath)
{
	raw_data = (unsigned int*) File_Utils::load_raw_asset(filepath);
	if(!raw_data)
	{
		LOGE("Error: failed to load \"%s\"\n",filepath);
		return 0;
	}

	//File Schematics
	//First float is the vertex count
	//Second float is the triangle count * 3
	//List thereafter is the position (3 floats) of all vertices
	//List thereafter is the uv coordinates (2 floats) of all vertices
	//List thereafter is the normals (3 floats) of all vertices
	//List thereafter is the tangents (3 floats) of all vertices
	//List thereafter is the binormals (3 floats) of all vertices
	//List thereafter is 3 bone indices for all vertices
	//List thereafter is 3 bone weights pertaining to the 3 bone indices defined above
	//List thereafter is the indices vertices that make up the triangles
	vertex_count = raw_data[0];
	tri_vert_count = raw_data[1];

	verts = (float*) raw_data + 2;
	uv_coords = (float*) raw_data + 2 + (vertex_count*3);
	normals = (float*) raw_data + 2 + (vertex_count*3) + (vertex_count * 2);
	tangents = (float*) raw_data + 2 + 2*(vertex_count*3) + (vertex_count * 2);
	binormals = (float*) raw_data + 2 + 3*(vertex_count*3) + (vertex_count * 2);
	bone_indices = (float*) raw_data + 2 + 4*(vertex_count*3) + (vertex_count * 2);
	bone_weights = (float*) raw_data + 2 + 5*(vertex_count*3) + (vertex_count * 2);

	tri_verts = raw_data + 2 + 6*(vertex_count*3) + (vertex_count*2);
	return 1;
}

int Skel_Model::init_gl()
{
	glGenBuffers(1, &tri_verts_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tri_verts_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(tri_vert_count * sizeof(unsigned int)), tri_verts, GL_STATIC_DRAW);

	return 1;
}

void Skel_Model::term_gl()
{
	glDeleteBuffers(1,&tri_verts_buffer);
}
void Skel_Model::unload_model()
{
	if(raw_data)
		free((void*)raw_data);
}

Skel_Model::Skel_Model(const char* filepath)
{
	load_model(filepath);
}

Skel_Model::~Skel_Model()
{
	unload_model();
}
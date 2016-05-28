//
// Created by F1 on 5/27/2016.
//

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

//Skeleton based-mesh, for static meshes, refer to todo: class Static_Mesh (remove Static_Model.cpp, once we get to implementation)
class Mesh
{
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

	int vertex_count;
	int tri_count;
	//arrays of vertex attributes
	//	uvs
	//	normals
	//		do we need tangents?
	//	positions (rest pose positions)

	//arrays of triangles as vert indices

	Material mat;


	//Has a function for rendering the model (binds all data)

	//Has a function for binding gl VBOs
	//Also has a function for unbinding gl VBOs


	//Should MeshUtils be its own class, or should we just add a method here for parsing a mesh file into a Mesh structure?


	//We will eventually need methods for combining Meshes, and copying Meshes

};

#endif //ENGINE_MESH_H

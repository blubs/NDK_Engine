//
// Created by F1 on 5/5/2016.
//

#ifndef ENGINE_STATIC_MODEL_H
#define ENGINE_STATIC_MODEL_H

//Class definition for models that have the following features
//No animations
//Vertices
//Triangles
//Normals
//UV1
//UV2
//... do we need tangents?
class Static_Model
{
public:
	char* data;
	//should we just reference everything?
	//or should we split up the references to normals, uvs, vertices as their own pointers?
};

#endif //ENGINE_STATIC_MODEL_H

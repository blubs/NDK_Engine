//
// Created by F1 on 6/3/2016.
//

#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "math/math.h"


//Outlines a class for a generic game entity
//Used to construct Entity hierarchy for parenting entities to each other
class Game_Object
{
public:
	Game_Object* parent = NULL;


	//Transform data
	Mat4 transform;
	Mat4 world_transform;
	//Reset to false at the beginning of each frame, denoting that we need to recalculate all world transforms
	bool transform_calculated = false;

	Vec3 pos;
	//Pitch, yaw, roll
	Vec3 angles;

	//Goes through parent hierarchy until parent is null, multiplying by all transformation matrices along the way
	virtual Mat4 get_world_transform() = 0;
};

//Used for entities that actually exist in the game world
class Entity : public Game_Object
{
public:
	Mat4 get_world_transform()
	{
		LOGE("get world transform started\n");
		if(transform_calculated)
		{
			return world_transform;
		}
		transform = Mat4::ROT_TRANS(angles,pos);

		if(parent)
			world_transform = parent->get_world_transform() * transform;
		else
			world_transform = transform;

		transform_calculated = true;
		LOGE("get world transform finished\n");
		return world_transform;
	}

};

#endif //ENGINE_ENTITY_H

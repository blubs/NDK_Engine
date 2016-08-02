//
// Created by F1 on 6/3/2016.
//

#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "math/math.hpp"
#include "graphics/Static_Model.hpp"

//Forward declaration
class Sound_Sample;

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
	//Whether to use the angles or the quaternion for rotation
	bool use_quaternion = false;
	Quat rot;

	//Goes through parent hierarchy until parent is null, multiplying by all transformation matrices along the way
	virtual Mat4 get_world_transform(bool modify_trans) = 0;

	Game_Object();
};

//Used for entities that actually exist in the game world
class Entity : public Game_Object
{
public:
	Static_Model* model = NULL;
	Material* mat = NULL;

	Mat4 get_world_transform(bool modify_trans);

	virtual int render(Mat4 vp);
	int play_sound(Sound_Sample* sample);
};

#endif //ENGINE_ENTITY_H

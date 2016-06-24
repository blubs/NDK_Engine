//
// Created by F1 on 6/3/2016.
//

#ifndef ENGINE_ENTITY_H
#define ENGINE_ENTITY_H

#include "math/math.h"
#include "Static_Model.h"

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

	//Goes through parent hierarchy until parent is null, multiplying by all transformation matrices along the way
	virtual Mat4 get_world_transform(bool modify_trans) = 0;
};

//Used for entities that actually exist in the game world
class Entity : public Game_Object
{
public:
	Static_Model* model = NULL;
	Material* mat = NULL;

	Mat4 get_world_transform(bool modify_trans)
	{
		if(transform_calculated)
		{
			return world_transform;
		}
		transform = Mat4::ROT_TRANS(angles,pos);

		if(parent)
			world_transform = parent->get_world_transform(modify_trans) * transform;
		else
			world_transform = transform;

		if(modify_trans)
			transform_calculated = true;
		return world_transform;
	}

	virtual int render(Mat4 vp)
	{
		if(!model)
			return 1;
		if(!mat)
			return 1;

		//LOGE("Material set: %p",mat);
		//LOGE("Material shader: %p, shader gl program: %d",mat->shader,mat->shader->gl_program);
		//LOGE("Model: %p, gl buffer %d",model,model->tri_verts_buffer);

		//Tri verts are good
		//LOGE("Printing mat params\n");
		//for(int i = 0; i < mat->shader->param_count; i++)
		//{
		//	LOGE("location: %d, type: %d\n",*(int*)(mat->shader->param_location[i]),mat->shader->param_type[i]);
		//}
		//Shader parameters and locations look good too...
		//next up to test: material params

		mat->bind_material();
		model->render(vp * get_world_transform(true),mat);
		return 1;
	}

	int play_sound(Sound_Sample* sample);

};

#endif //ENGINE_ENTITY_H

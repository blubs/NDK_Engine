//
// Created by F1 on 6/3/2016.
//

#include "Game_Object.hpp"
//Placing include here to avoid circular dependency
#include "sound/Audio_Engine.hpp"

Game_Object::Game_Object()
{
	transform = Mat4::IDENTITY();
	world_transform = Mat4::IDENTITY();
	pos = Vec3::ZERO();
	angles = Vec3::ZERO();
	rot = Quat(0.0f,Vec3::RIGHT());
	use_quaternion = false;
}

Mat4 Entity::get_world_transform(bool modify_trans)
{
	if(transform_calculated && modify_trans)
	{
		return world_transform;
	}
		if(use_quaternion)
		transform = Mat4::ROT_TRANS(rot,pos);
	else
		transform = Mat4::ROT_TRANS(angles,pos);
		if(parent)
		world_transform = parent->get_world_transform(modify_trans) * transform;
	else
		world_transform = transform;
		if(modify_trans)
		transform_calculated = true;
	return world_transform;
}

int Entity::render(Mat4 vp)
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
	model->render(get_world_transform(true),vp,mat);
	return 1;
}

int Entity::play_sound(Sound_Sample* sample)
{
	if(!sample)
	{
		LOGW("Warning: tried playing sound with null sample");
		return 1;
	}

	Audio_Engine::play_sound(sample,this,Vec3::ZERO(),0,1.0f);

	return 1;
}

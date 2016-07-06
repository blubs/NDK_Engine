//
// Created by F1 on 4/29/2016.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include "Game_Object.h"
#include "Skel_Model.h"

class Player : public Entity
{
public:
	Skeleton* skel;
	//Have to store an array of all models that make up the player
	Skel_Model* player_model;

	int render(Mat4 vp, float TEMP_VAR)
	{
		if(!mat)
			return 1;

		//TODO: render all playermodels
		mat->bind_material();
		skel->update_frame(TEMP_VAR);
		player_model->render(get_world_transform(true),vp,mat);
		return 1;
	}
};

#endif //ENGINE_PLAYER_H

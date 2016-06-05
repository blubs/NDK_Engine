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
	Skel_Model* model;

	int render(Mat4 vp)
	{

		if(!skel->playing_anim)
		{
			skel->play_anim(0);
		}

		//TODO: render all playermodels
		skel->update_frame();
		model->render(vp * get_world_transform());
		return 1;
	}
};

#endif //ENGINE_PLAYER_H

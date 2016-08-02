//
// Created by F1 on 4/29/2016.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include "../engine/Game_Object.hpp"
#include "../engine/graphics/Skel_Model.hpp"

class Player : public Entity
{
public:
	Skeleton* skel;
	//Have to store an array of all models that make up the player
	Skel_Model* player_model;

	int render(Mat4 vp);

	//Update method that is ran every frame
	//Most of the actual game logic is contained here
	int update();
};

#endif //ENGINE_PLAYER_H

//
// Created by F1 on 4/29/2016.
//

#include "Player.hpp"

int Player::render(Mat4 vp)
{
	if(!mat)
		return 1;

	//TODO: render all playermodels
	mat->bind_material();
	skel->update_frame();
	player_model->render(get_world_transform(true),vp,mat);
	return 1;
}

//Update method that is ran every frame
//Most of the actual game logic is contained here
int Player::update()
{
	//6 m/s * frame time
	pos.y += 6.0f * 1/60.0f;

	if(pos.y >= 20)
	{
		pos.y = -20;
	}
	return 1;
}
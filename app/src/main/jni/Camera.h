//
// Created by F1 on 4/29/2016.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "common.h"
#include "Game_Object.h"

class Camera : public Entity
{
public:
	float fov;
	float aspect;
	float near_plane;
	float far_plane;


	Vec3 up;
	Vec3 forward;
	Vec3 right;

	//How are we going to reference the parent of the camera?

	Mat4 persp_proj_m;
	Mat4 ortho_proj_m;
	Mat4 inf_proj_m;
	Mat4 view_m;

	//Update the view attributes
	void set_persp_view(float field_of_view, int width, int height, float near_clipping_plane, float far_clipping_plane)
	{
		fov = field_of_view;
		float aspect_ratio = ((float) width )/ ((float) height);
		aspect = aspect_ratio;
		near_plane = near_clipping_plane;
		far_plane = far_clipping_plane;

		//Calculate the projection matrix
		persp_proj_m = Mat4::PROJECT_PERSPECTIVE(near_plane,far_plane,aspect,fov);
		//Calculate the infinite projection matrix
		inf_proj_m = Mat4::PROJECT_INFINITE(near_plane,aspect,fov);
	}
	void set_ortho_view(int width, int height, float near_clipping_plane, float far_clipping_plane)
	{
		//Calculate the projection matrix
		ortho_proj_m = Mat4::PROJECT_ORTHO(near_clipping_plane,far_clipping_plane,(float)width,(float)height);
	}


	//Converts angles to forward,right,and up vectors, and sets the view matrix
	void update_view_matrix()
	{
		Quat flip_y_and_z(HALF_PI,Vec3::RIGHT());

		transform = Mat4::ROT_TRANS(angles,pos,&right,&up,&forward) * Mat4::ROTATE(flip_y_and_z);
		if(parent)
			world_transform = parent->get_world_transform(true) * transform;
		else
			world_transform = transform;

		view_m = world_transform.inverted();
	}
};

#endif //ENGINE_CAMERA_H


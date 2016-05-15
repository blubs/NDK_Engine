//
// Created by F1 on 4/29/2016.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "common.h"

class Camera
{
public:
	float fov;
	float aspect;
	float near_plane;
	float far_plane;


	Vec3 pos;
	//Pitch, yaw, roll
	Vec3 angles;

	Vec3 up;
	Vec3 forward;
	Vec3 right;

	//How are we going to reference the parent of the camera?

	Mat4 projection_m;
	Mat4 view_m;

	//Update the view attributes
	void set_view_attributes(float field_of_view, float aspect_ratio, float near_clipping_plane, float far_clipping_plane)
	{
		fov = field_of_view;
		aspect = aspect_ratio;
		near_plane = near_clipping_plane;
		far_plane = far_clipping_plane;

		//Calculate the projection matrix
		projection_m = Mat4::PROJECT_PERSPECTIVE(near_plane,far_plane,aspect,fov);
	}

	//Converts angles to forward,right,and up vectors, and sets the view matrix
	void update_view_matrix()
	{
		//Rotating angles.x about RIGHT vector for pitch
		Quat pitch(angles.x, Vec3::RIGHT());
		//Rotating angles.y about UP vector for yaw
		Quat yaw(angles.y, Vec3::UP());

		Quat rot = yaw*pitch;
		forward = rot * Vec3::FRONT();

		//Rotating angles.z about the forward vector for the roll
		Quat roll(angles.z, forward);
		//Adding roll to pitch * yaw
		rot = roll * rot;
		right = rot * Vec3::RIGHT();
		up = Vec3::cross(right,forward);
		view_m = Mat4::VIEW(right,up,forward,pos);
	}
};

#endif //ENGINE_CAMERA_H


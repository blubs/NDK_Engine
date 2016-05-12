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
		projection_m = Mat4::PROJECT(near_plane,far_plane,aspect,fov);
	}

	//Converts angles to forward,right,and up vectors, and sets the view matrix
	void update_view_matrix()
	{
		/*
		//Rotating angles.y about UP vector for yaw
		Quat yaw(angles.y, Vec3::UP());
		//Rotating angles.x about RIGHT vector for pitch
		Quat pitch(angles.x, yaw*Vec3::RIGHT());

		Quat rot = pitch*yaw;
		forward = rot * Vec3::FRONT();
		LOGE("FRONT VEC IS : (%.2f,%.2f,%.2f)\n",forward.x,forward.y,forward.z);

		//Right vector calculated forward cross global up
		right = Vec3::cross(forward,Vec3::UP());
		LOGE("RIGHT VEC IS : (%.2f,%.2f,%.2f)\n",right.x,right.y,right.z);

		//Up vector calculated from right cross forward
		up = Vec3::cross(right,forward);
		LOGE("UP VEC IS : (%.2f,%.2f,%.2f)\n",up.x,up.y,up.z);

		view_m = Mat4::VIEW(right,up,forward,pos);*/
		/*

		//Rotating angles.z about the forward vector for the roll
		Quat roll(angles.z, forward);
		//Adding roll to pitch * yaw
		rot = roll * rot;
		right = rot * Vec3::RIGHT();
		//Vec3 up = result * Vec3::UP(); // has the same result as line below
		up = Vec3::cross(right,forward);
		//Previously I needed this... BUT
		view_m = Mat4::VIEW(right,up,forward,pos);*/
		//going to test this
		view_m = Mat4::VIEW(angles,pos);
	}
};

#endif //ENGINE_CAMERA_H


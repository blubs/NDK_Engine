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
	Vec3 angles;

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
};

#endif //ENGINE_CAMERA_H


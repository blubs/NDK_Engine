//
// Created by F1 on 4/29/2016.
//

#include "Camera.h"

//Update the view attributes
void Camera::set_persp_view(float field_of_view, int width, int height, float near_clipping_plane, float far_clipping_plane)
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
void Camera::set_ortho_view(int width, int height, float near_clipping_plane, float far_clipping_plane)
{
	//Calculate the projection matrix
	ortho_proj_m = Mat4::PROJECT_ORTHO(near_clipping_plane,far_clipping_plane,(float)width,(float)height);
}


//Converts angles to forward,right,and up vectors, and sets the view matrix
void Camera::update_view_matrix()
{
	Quat flip_y_and_z(HALF_PI,Vec3::RIGHT());

	if(use_quaternion)
		transform = Mat4::ROT_TRANS(rot,pos,&right,&up,&forward) * Mat4::ROTATE(flip_y_and_z);
	else
		transform = Mat4::ROT_TRANS(angles,pos,&right,&up,&forward) * Mat4::ROTATE(flip_y_and_z);

	if(parent)
		world_transform = parent->get_world_transform(true) * transform;
	else
		world_transform = transform;

	view_m = world_transform.inverted();
}

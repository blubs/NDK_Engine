//
// Created by F1 on 6/14/2016.
//

#include "UI_Image.h"

int UI_Image::render(Mat4 vp)
{
	if(!visible)
		return 1;

	//Default quad is facing the positive z axis direction
	const float quad_verts[] =
	{
	-0.5f,0.5f,0.0f,
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.5f,0.5f,0.0f,
	-0.5f,0.5f,0.0f
	};

	float top = 1.0f - uv_maxs.y;
	float bottom = 1.0f - uv_mins.y;

	const float quad_uvs[] =
	{
		uv_mins.x,top,
		uv_mins.x,bottom,
		uv_maxs.x,bottom,
		uv_maxs.x,bottom,
		uv_maxs.x,top,
		uv_mins.x,top
	};

	const float mult_col[] = { color.x,color.y,color.z,opacity };
	const float add_col[] = { add_color.x,add_color.y,add_color.z,0.0f };

	float vertical_scale = 1.0f;
	if(!maintain_aspect_ratio)
		vertical_scale = scale.y;
	else
		vertical_scale = scale.x * ((uv_maxs.y - uv_mins.y)/(uv_maxs.x - uv_mins.x));

	Mat4 mvp = vp * Mat4::ROT_TRANS(angles,pos) * Mat4::SCALE(scale.x,vertical_scale,1.0f);
	mat->bind_material();
	mat->bind_value(Shader::PARAM_VERTICES,(void*) quad_verts);
	mat->bind_value(Shader::PARAM_VERT_UV1,(void*) quad_uvs);
	mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) tex->gl_id);
	mat->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);

	mat->bind_value(Shader::PARAM_COLOR_MULT, (void *) mult_col);
	mat->bind_value(Shader::PARAM_COLOR_ADD, (void *) add_col);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	return 1;
}

int UI_Image::init(Material* m, Texture* texture)
{
	text = NULL;
	text_length = 0;
	pos = Vec3(0.0f,0.0f,0.5f);
	angles = Vec3::ZERO();
	color = Vec3(1.0f,1.0f,1.0f);
	add_color = Vec3(0.0f,0.0f,0.0f);

	uv_mins = Vec3(0.0f,0.0f,0.0f);
	uv_maxs = Vec3(1.0f,1.0f,0.0f);

	opacity = 1.0f;
	mat = m;
	tex = texture;
	visible = true;


	scale = Vec3(1.0f,1.0f,0.0f);

	maintain_aspect_ratio = false;


	return 1;
}
void UI_Image::term()
{
}
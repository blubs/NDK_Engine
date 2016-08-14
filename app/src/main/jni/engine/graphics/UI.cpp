//
// Created by F1 on 6/7/2016.
//

#include "UI.hpp"

//============================ UI_Text method implementations ==============================

//From ASCII value - 32 (32 is space, which isn't mapped)
//to ASCII value 126
//Showing what char goes at which index
//{!,",#,$,%,&,',(,),*,+,,,-,.,/,0,1,2,3,4,5,6,7,8,9,:,;,<,=,>,?,@,ABCDEFGHIJKLMNOPQRSTUVWXYZ,[,\,],^,_,`,abcdefghijklmnopqrstuvwxyz,{,|,},~}
const char UI_Text::char_index_x[94] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3};
const char UI_Text::char_index_y[94] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,8,9,9,9,9};
//The following two arrays were built by hand using tedious trial and error
	//I had all of the characters printed to the screen, along with a bounding box around the character
	//whose left border was shifted right by the pre_space amount, and whose right border was shifted left by the suf_space amount
	//I then adjusted the values per character in order to achieve a uniform thin space on either side of the character
//Spacing to take away before the char
float UI_Text::char_pre_space[94] =
{
	0.4,0.3,0.1,0.25,0.05,0.15,0.4,0.25,0.35,0.13,
	0.06,0.35,0.25,0.4,0.15,0.15,0.35,0.25,0.25,0.15,
	0.25,0.15,0.14,0.15,0.18,0.4,0.33,0.15,0.2,0.15,
	0.25,0.08,0.1,0.19,0.1,0.18,0.15,0.2,0.15,0.16,
	0.3,0.2,0.15,0.17,0.1,0.02,0.15,0.15,0.07,0.15,
	0.15,0.05,0.1,0.05,0,0.05,0.12,0.02,0.25,0.15,
	0.25,0.1,0.03,0.35,0.2,0.2,0.2,0.15,0.15,0.1,
	0.15,0.15,0.25,0.09,0.1,0.33,0.05,0.11,0.13,0.13,
	0.2,0.17,0.15,0.2,0.1,0.1,0,0.05,0.08,0.15,
	0.2,0.35,0.3,0.12
};
//Spacing to take away from the end of the char
float UI_Text::char_suf_space[94] =
{
	0.38,0.3,0.1,0.2,0.05,0.15,0.4,0.35,0.3,0.13,
	0.06,0.35,0.25,0.4,0.2,0.2,0.35,0.25,0.25,0.15,
	0.2,0.15,0.2,0.15,0.27,0.35,0.35,0.1,0.2,0.1,
	0.2,0.05,0.08,0.23,0.2,0.05,0.18,0.22,0.07,0.17,
	0.35,0.25,0.15,0.12,0.1,0.3,0.1,0.2,0.07,0.17,
	0.25,0.05,0.1,0.05,0,0.05,0.05,0.12,0.25,0.15,
	0.26,0.05,0.03,0.3,0.25,0.2,0.2,0.25,0.18,0.21,
	0.18,0.15,0.26,0.27,0.14,0.28,0.07,0.18,0.13,0.13,
	0.15,0.15,0.15,0.1,0.1,0.1,0,0.1,0.11,0.12,
	0.35,0.32,0.22,0.1
};

int UI_Text::render(Mat4 vp)
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

	Vec3 char_ofs = Vec3::ZERO();

	for(int i = 0; i < text_length; i++)
	{
		char c = text[i];
		if(c == '\n')
		{
			char_ofs.x = 0.0f;
			char_ofs.y -= 1.0f;
			continue;
		}


		if(c <= ' ' || c > '~')
		{
			char_ofs.x += 0.9f;
			continue;
		}
		c = c - '!';

		float index_x = char_index_x[c];
		float index_y = char_index_y[c];

		float cell_size = 0.09765625f;//this is 10 cells of size 100 px fitting in 1024 pixels, with 24 wasted pixels

		float corner_x = cell_size * index_x;
		float corner_y = cell_size * index_y;

		float left = corner_x;
		float top = corner_y;
		float bottom = corner_y + cell_size;
		float right = corner_x + cell_size;

		const float quad_uvs[] =
		{
		left,top,
		left,bottom,
		right,bottom,
		right,bottom,
		right,top,
		left,top
		};

		if(!monospaced)
		{
			char_ofs.x -= char_pre_space[c];
		}

		const float mult_col[] = { color.x,color.y,color.z,opacity };
		const float add_col[] = { add_color.x,add_color.y,add_color.z,0.0f };

		Mat4 mvp = vp * Mat4::ROT_TRANS(angles,pos) * Mat4::SCALE(font_size,font_size,font_size) * Mat4::TRANSLATE(char_ofs);
		mat->bind_material();
		mat->bind_value(Shader::PARAM_VERTICES,(void*) quad_verts);
		mat->bind_value(Shader::PARAM_VERT_UV1,(void*) quad_uvs);
		mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) charset);
		mat->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);

		mat->bind_value(Shader::PARAM_COLOR_MULT, (void *) mult_col);
		mat->bind_value(Shader::PARAM_COLOR_ADD, (void *) add_col);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		if(monospaced)
			char_ofs.x += 1.1f;
		else
			char_ofs.x += 1.0f - char_suf_space[c];
	}

	return 1;
}

int UI_Text::set_text(const char* text_to_set)
{
	if(text)
		clear_text();

	text = (char*) malloc(sizeof(char) * (strlen(text_to_set)+1));
	strcpy(text,text_to_set);
	text_length = strlen(text);
	visible = true;
	return 1;
}

void UI_Text::clear_text()
{
	visible = false;
	if(text)
	{
		free(text);
	}
}

int UI_Text::init(Material* m, Texture* tex)
{
	text = NULL;
	text_length = 0;
	pos = Vec3(0.0f,0.0f,0.5f);
	angles = Vec3::ZERO();
	color = Vec3(1.0f,1.0f,1.0f);
	add_color = Vec3(0.0f,0.0f,0.0f);

	opacity = 1.0f;
	visible = false;
	font_size = 100.0f;
	mat = m;
	charset = tex;
	monospaced = false;

	return 1;
}
void UI_Text::term()
{
	clear_text();
}

UI_Text::UI_Text (Material *m, Texture *tex)
{
	init(m,tex);
}

UI_Text::~UI_Text()
{
	term();
}


//============================ UI_Image method implementations ==============================
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
	mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) tex);
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

UI_Image::UI_Image(Material* m, Texture* tex)
{
	init(m,tex);
}

UI_Image::~UI_Image()
{
	term();
}
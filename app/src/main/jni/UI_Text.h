//
// Created by F1 on 6/7/2016.
//

#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include "math/math.h"
#include "Material.h"
#include "Texture.h"

class UI_Text
{
public:
	//Null terminated text
	char* text;
	//number of characters in text
	int text_length;

	Vec3 pos;
	Vec3 angles;//application of these angles might depend on whether we are in global or view space
	//if this were a mere texture, it would make sense to do mins/maxs and options to maintain the aspect ratio of an image portion
	//but because this is text (a composition of different squares that are mapped onto individual letters)
	//modulating the size/scale of the text may be an issue
	//Could do this:
	//Vec3 scale;
	//or:
	//Vec3 mins;
	//Vec3 maxs;
	//Or some other alternative:

	float font_size;

	Vec3 color;//color to multiply texture by
	Vec3 add_color;

	float opacity;

	bool visible;


	Material* mat;
	Texture* charset;

	//From ASCII value - 32 (32 is space, which isn't mapped)
	//to ASCII value 126
	//Showing what char goes at which index
	//{!,",#,$,%,&,',(,),*,+,,,-,.,/,0,1,2,3,4,5,6,7,8,9,:,;,<,=,>,?,@,ABCDEFGHIJKLMNOPQRSTUVWXYZ,[,\,],^,_,`,abcdefghijklmnopqrstuvwxyz,{,|,},~}
	static const char char_index_x[94];
	static const char char_index_y[94];

	//Spacing before before
	static float char_pre_space[94];
	//Spacing after char
	static float char_suf_space[94];

	int render(Mat4 vp)
	{
		if(!visible)
			return 1;

		//TODO: parse the character that is in the text, and draw the right character from the charset (derive the right uvs)
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

			{
				//TEMPORARY DRAWING OF BOUNDS:
				//======================================================================================================================
				float semi_rand1 = ((i * 37447) % 38217) / 38217.0f;
				float semi_rand2 = (((i+57503) * 12163) % 11287) / 11287.0f;
				float semi_rand3 = (((i+69247) * 56527) % 57487) / 57487.0f;

				const float mult_col2[] = { 0.0f,0.0f,0.0f,0.0f };
				const float add_col2[] = { semi_rand1,semi_rand2,semi_rand3,1.0f };

				float char_size = 1.0f - (char_pre_space[c] + char_suf_space[c]);

				Mat4 mvp = vp * Mat4::ROT_TRANS(angles,pos) * Mat4::SCALE(font_size,font_size,font_size) * Mat4::SCALE(char_size,1.0f,1.0f) * Mat4::TRANSLATE(char_ofs + Vec3(char_pre_space[c],0.0f,0.0f));
				mat->bind_material();
				mat->bind_value(Shader::PARAM_VERTICES,(void*) quad_verts);
				mat->bind_value(Shader::PARAM_VERT_UV1,(void*) quad_uvs);
				mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) charset->gl_id);
				mat->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);

				mat->bind_value(Shader::PARAM_COLOR_MULT, (void *) mult_col2);
				mat->bind_value(Shader::PARAM_COLOR_ADD, (void *) add_col2);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				//=====================================================================================================================
			}

			const float mult_col[] = { color.x,color.y,color.z,opacity };
			const float add_col[] = { add_color.x,add_color.y,add_color.z,0.0f };

			Mat4 mvp = vp * Mat4::ROT_TRANS(angles,pos) * Mat4::SCALE(font_size,font_size,font_size) * Mat4::TRANSLATE(char_ofs);
			mat->bind_material();
			mat->bind_value(Shader::PARAM_VERTICES,(void*) quad_verts);
			mat->bind_value(Shader::PARAM_VERT_UV1,(void*) quad_uvs);
			mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) charset->gl_id);
			mat->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);

			mat->bind_value(Shader::PARAM_COLOR_MULT, (void *) mult_col);
			mat->bind_value(Shader::PARAM_COLOR_ADD, (void *) add_col);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			char_ofs.x += 1.1f;
		}



		return 1;
	}

	int set_text(const char* text_to_set)
	{
		if(text)
			clear_text();

		text = (char*) malloc(sizeof(char) * (strlen(text_to_set)+1));
		strcpy(text,text_to_set);
		text_length = strlen(text);
		visible = true;
		return 1;
	}

	void clear_text()
	{
		visible = false;
		if(text)
		{
			free(text);
		}
	}

	int init(Material* m, Texture* tex)
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

		return 1;
	}
	void term()
	{
		clear_text();
	}
};

#endif //ENGINE_UI_H

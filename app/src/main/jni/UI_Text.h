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

	Vec3 color;//color to multiply texture by
	Vec3 add_color;

	float opacity;

	bool visible;


	Material* mat;
	Texture* charset;

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

		for(int i = 0; i < text_length; i++)
		{
			//TODO: get uv coordinates of char from charset
			char c = text[i];
			if(c == ' ')
				continue;


			float letters_per_row = 32.0f;

			float index_x = c - 'a';
			float index_y = (index_x/letters_per_row);
			index_x = fmodf(index_x,1.0f);

			float cell_size = 1.0f/letters_per_row;

			float center_x = cell_size * index_x;
			float center_y = cell_size * index_y;

			cell_size = 1.0f;
			center_x = center_y = 0.5f;

			float left = center_x - cell_size*0.5f;
			float top = center_y + cell_size*0.5f;
			float bottom = center_y - cell_size*0.5f;
			float right = center_x + cell_size*0.5f;

			//Images are flipped
			top = 1.0f - top;
			bottom = 1.0f - bottom;

			Vec3 letter_offset(i*1.1f,0.0f,0.0f);

			const float quad_uvs[] =
			{
				left,top,
				left,bottom,
				right,bottom,
				right,bottom,
				right,top,
				left,top
			};

			const float mult_col[] = { color.x,color.y,color.z,opacity };
			const float add_col[] = { add_color.x,add_color.y,add_color.z,0.0f };

			Mat4 mvp = vp * Mat4::ROT_TRANS(angles,pos) * Mat4::SCALE(100.0f,100.0f,100.0f) * Mat4::TRANSLATE(letter_offset);
			mat->bind_material();
			mat->bind_value(Shader::PARAM_VERTICES,(void*) quad_verts);
			mat->bind_value(Shader::PARAM_VERT_UV1,(void*) quad_uvs);
			mat->bind_value(Shader::PARAM_TEXTURE_DIFFUSE, (void *) charset->gl_id);
			mat->bind_value(Shader::PARAM_MVP_MATRIX, (void *) mvp.m);

			mat->bind_value(Shader::PARAM_COLOR_MULT, (void *) mult_col);
			mat->bind_value(Shader::PARAM_COLOR_ADD, (void *) add_col);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}



		return 1;
	}

	int set_text(const char* text_to_set)
	{
		if(text)
			clear_text();

		text = (char*) malloc(sizeof(char) * sizeof(text_to_set));
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

	//Beginning with drawing text to the screen
	//	Need to load characters texture and initialize the gl side of it
	//	Need to create a method that parses a string and
	//	Need to create arrays that hold information about character spacing/size/where it is on the charset
	//	Need to create a method for drawing a generic quad to the screen

	int init(Material* m, Texture* tex)
	{
		text = NULL;
		text_length = 0;
		//pos = Vec3(0.5f * 1440.0f,0.5f * 2560.0f,0.5f);
		pos = Vec3(-500.0f,0.0f,0.5f);
		angles = Vec3::ZERO();
		color = Vec3(1.0f,1.0f,1.0f);
		add_color = Vec3::ZERO();
		opacity = 1.0f;
		visible = false;

		mat = m;
		charset = tex;

		return 1;
	}
	void term()
	{
	}
	int init_gl()
	{
		return 1;
	}
	void term_gl()
	{
	}
};

#endif //ENGINE_UI_H

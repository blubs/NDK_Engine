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

		for(int i = 0; i < text_length; i++)
		{
			char c = text[i];
			if(c <= ' ' || c > '~')
				continue;
			c = c - '!';

			float index_x = char_index_x[c];
			float index_y = char_index_y[c];

			//LOGE("place of char: %c, (%f,%f)\n",(c+'!'),index_x,index_y);

			float cell_size = 0.09765625f;//this is 10 cells of size 100 px fitting in 1024 pixels, with 24 wasted pixels

			float corner_x = cell_size * index_x;
			float corner_y = cell_size * index_y;

			float left = corner_x;
			float top = corner_y;
			float bottom = corner_y + cell_size;
			float right = corner_x + cell_size;

			Vec3 letter_offset(i*0.9f,0.0f,0.0f);

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
		//This has very ugly results actually
		//color = Vec3(0.0f,0.0f,0.0f);
		//add_color = Vec3(1.0f,1.0f,1.0f);
		color = Vec3(1.0f,1.0f,1.0f);
		add_color = Vec3(0.0f,0.0f,0.0f);

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

//
// Created by F1 on 6/7/2016.
//

#ifndef ENGINE_UI_H
#define ENGINE_UI_H

#include "../math/math.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class UI_Text
{
public:
	//Null terminated text
	char* text;
	//number of characters in text
	int text_length;

	Vec3 pos;
	Vec3 angles;

	float font_size;
	bool monospaced;

	Vec3 color;//color to multiply texture by
	Vec3 add_color;

	float opacity;

	bool visible;


	Material* mat;
	Texture* charset;

	//From ASCII value - 32 (32 is space, which isn't mapped)
	//to ASCII value 126
	//Showing what char goes at which index
	//The following is a list of all characters as they appear on the charset
	//there is a newline character every ten chars, to match the charset
	//"!\"#$%&'()*\n+,-./01234\n56789:;<=>\n?@ABCDEFGH\nIJKLMNOPQR\nSTUVWXYZ[\\\n]^_`abcdef\nghijklmnop\nqrstuvwxyz\n{|}~"
	//{!,",#,$,%,&,',(,),*,+,,,-,.,/,0,1,2,3,4,5,6,7,8,9,:,;,<,=,>,?,@,ABCDEFGHIJKLMNOPQRSTUVWXYZ,[,\,],^,_,`,abcdefghijklmnopqrstuvwxyz,{,|,},~}
	static const char char_index_x[94];
	static const char char_index_y[94];

	//Spacing before before
	static float char_pre_space[94];
	//Spacing after char
	static float char_suf_space[94];

	int render(Mat4 vp);

	int set_text(const char* text_to_set);

	void clear_text();
	UI_Text(Material* m, Texture* tex);
	~UI_Text();

private:
	int init(Material* m, Texture* tex);
	void term();
};

class UI_Image
{
public:
	//Null terminated text
	char* text;
	//number of characters in text
	int text_length;

	Vec3 pos;
	Vec3 angles;
	Vec3 scale;
	//If maintain_aspect_ratio is true, the y scale is ignored,
	// 	instead we scale the quad with the x scale, maintaining the aspect ratio given by the uv_mins and uv_maxs for the y scale
	bool maintain_aspect_ratio;

	Vec3 uv_mins;
	Vec3 uv_maxs;

	Vec3 color;//color to multiply texture by
	Vec3 add_color;

	float opacity;

	bool visible;


	Material* mat;
	Texture* tex;

	int render(Mat4 vp);

	UI_Image(Material* m, Texture* tex);
	~UI_Image();

private:
	int init(Material* m, Texture* tex);
	void term();
	//TODO: consider adding animated sprite sheet support if we ever find ourselves in need of them
};

#endif //ENGINE_UI_H

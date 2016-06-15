//
// Created by F1 on 6/14/2016.
//

#ifndef ENGINE_UI_IMAGE_H
#define ENGINE_UI_IMAGE_H

#include "math/math.h"
#include "Material.h"
#include "Texture.h"

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

	int init(Material* m, Texture* tex);
	void term();
//TODO: consider adding animated sprite sheet support if we ever find ourselves in need of them
};

#endif //ENGINE_UI_IMAGE_H

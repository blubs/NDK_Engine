//
// Created by F1 on 6/5/2016.
//

#include "Texture.hpp"

int Texture::load(const char* filepath,int w, int h)
{
	const Asset_File* file_asset = File_Utils::load_asset(filepath);
	raw_data_length = file_asset->len;
	raw_data = file_asset->data;
	free((void*) file_asset);
	if(!raw_data)
	{
		LOGE("Error: failed to load \"%s\"\n",filepath);
		return 0;
	}
	width = w;
	height = h;
	return 1;
}
void Texture::unload()
{
	if(raw_data)
		free((void*)raw_data);
	raw_data_length = 0;
	width = height = 0;
}

Texture::Texture(const char* filepath,int w, int h)
{
	load(filepath,w,h);
}

Texture::~Texture()
{
	unload();
}

int Texture::init_gl()
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC1_RGB8, width, height, 0, raw_data_length, (const void *) raw_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	gl_id = tex_id;
	return 1;
}

void Texture::term_gl()
{
	if(gl_id)
	{
		glDeleteTextures(1, &gl_id);
	}
	gl_id = 0;
}

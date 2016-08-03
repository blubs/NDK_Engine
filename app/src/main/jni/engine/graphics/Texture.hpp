//
// Created by F1 on 6/5/2016.
//

#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#include "../common.hpp"
#include "../File_Utils.hpp"

class Texture
{
public:
	const char* raw_data;
	GLuint gl_id = 0;
	int raw_data_length = 0;
	int width,height = 0;
	int init_gl();
	void term_gl();

	Texture(const char* filepath,int w, int h);
	~Texture();
private:
	int load(const char* filepath,int w, int h);
	void unload();
};

#endif //ENGINE_TEXTURE_H

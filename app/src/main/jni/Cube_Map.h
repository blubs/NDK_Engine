//
// Created by F1 on 6/30/2016.
//

#ifndef ENGINE_CUBE_MAP_H
#define ENGINE_CUBE_MAP_H

#include "common.h"
#include "File_Utils.h"


//misc TODO: add a Skybox class that holds a reference to a cube_map and renders it
//misc TODO: add the infinite projection matrix to Mat4 struct and camera
//mist TODO: add a Skybox shader

class Cube_Map
{
public:
	GLuint gl_id = 0;

	const char* raw_data_pos_x;
	int raw_data_p_x_length = 0;
	const char* raw_data_pos_y;
	int raw_data_p_y_length = 0;
	const char* raw_data_pos_z;
	int raw_data_p_z_length = 0;
	int width,height = 0;

	//FIXME remove this stuff (these are sorted)
	//LOGE("pos x: %d",GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	//LOGE("neg x: %d",GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	//LOGE("pos y: %d",GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	//LOGE("neg y: %d",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	//LOGE("pos z: %d",GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	//LOGE("neg z: %d",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	//=======================

	int load(const char* filepath,int w, int h)
	{
		//Order of constats: +x -x +y -y +z -z//(in case we need it)
		//Add a suffix before the extension for the filenames of the cube map name
		//if salad.pkm is passed, actual filenames are: salad_X.pkm where "X" is replaced with any of the following "l","r","u","d","f", and "b"
		int filename_length = strlen(filepath) + 1;//1 for null char

		char* file_name = (char*) malloc(sizeof(char) * (filename_length + 2));//2 extra chars for the added extension

		//Index of the letter we are going to modify
		int index_of_identifier = 0;

		//Copying the string up until the period, then inserting the added extension
		for(int i = 0; i < filename_length; i++)
		{
			if(filepath[i] == '.')
			{
				index_of_identifier = i + 1;
				file_name[i] = '_';
				file_name[i+1] = 'l';
				break;
			}
			file_name[i] = filepath[i];
		}

		//Finish copying the string
		for(int i = index_of_identifier - 1; i < filename_length; i++)
		{
			file_name[i+2] = filepath[i];
		}

		LOGE("%s  -->  %s\n",filepath,file_name);

		//TODO: load the six cubemap images

		/*const Asset_File* file_asset = File_Utils::load_asset(filepath);
		raw_data_p_x_length = file_asset->len;
		raw_data_pos_x = file_asset->data;
		free((void*) file_asset);
		if(!raw_data_pos_x)
		{
			LOGE("Error: failed to load \"%s\"\n",filepath);
			return 0;
		}
		width = w;
		height = h;*/

		free(file_name);
		return 1;
	}
	void unload()
	{
		//TODO: unload the six cubemap images
		//if(raw_data_pos_x)
		//	free((void*)raw_data_pos_x);
		//raw_data_p_x_length = 0;
		width = height = 0;
	}

	int init_gl()
	{
		//TODO: initialize the cubemap and pass in the 6 images
		GLuint tex_id;
		glGenTextures(1, &tex_id);

		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC1_RGB8, width, height, 0, raw_data_p_x_length, (const void *) raw_data_pos_x);
		glBindTexture(GL_TEXTURE_2D, 0);

		gl_id = tex_id;
		return 1;
	}

	void term_gl()
	{
		//TODO: terminate the cubemap
		if(gl_id)
		{
			glDeleteTextures(1, &gl_id);
		}
		gl_id = 0;
	}
};

#endif //ENGINE_CUBE_MAP_H

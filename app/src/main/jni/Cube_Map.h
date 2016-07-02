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

	const char* raw_data_neg_x;
	int raw_data_n_x_length = 0;

	const char* raw_data_pos_y;
	int raw_data_p_y_length = 0;

	const char* raw_data_neg_y;
	int raw_data_n_y_length = 0;

	const char* raw_data_pos_z;
	int raw_data_p_z_length = 0;

	const char* raw_data_neg_z;
	int raw_data_n_z_length = 0;

	int width,height = 0;

	int load_single_image(const char* filepath, const char** data, int* len)
	{
		const Asset_File* file_asset = File_Utils::load_asset(filepath);
		*data = file_asset->data;
		*len = file_asset->len;

		//raw_data_p_x_length = file_asset->len;
		//raw_data_pos_x = file_asset->data;

		free((void*) file_asset);
		if(!*data)
		{
			LOGE("Error: failed to load image: \"%s\"\n",filepath);
			unload();
			return 0;
		}
		return 1;
	}
	int load(const char* filepath,int w)
	{
		LOGI("cubemap load started");
		//Add a suffix before the extension for the filenames of the cube map name
		//Example: if salad.pkm is passed,
		//	actual filenames are: salad_X.pkm where "X" is replaced with any of the following "l","r","u","d","f", and "b"

		int filename_length = strlen(filepath) + 1;//1 for null char

		char file_name[filename_length + 2];

		//Index of the letter we are going to modify
		int index_of_identifier = 0;

		//Copying the string up until the period, then inserting the added extension
		for(int i = 0; i < filename_length; i++)
		{
			if(filepath[i] == '.')
			{
				index_of_identifier = i + 1;
				file_name[i] = '_';
				file_name[i+1] = 'X';
				break;
			}
			file_name[i] = filepath[i];
		}

		//Finish copying the string
		for(int i = index_of_identifier - 1; i < filename_length; i++)
		{
			file_name[i+2] = filepath[i];
		}

		//======== Actually loading the images =========
		//=== Loading the positive x-axis image ===
		file_name[index_of_identifier] = 'r';
		if(!load_single_image(file_name,&raw_data_pos_x,&raw_data_p_x_length))
			return 0;
		//=== Loading the negative x-axis image ===
		file_name[index_of_identifier] = 'l';
		if(!load_single_image(file_name,&raw_data_neg_x,&raw_data_n_x_length))
			return 0;
		//=== Loading the positive y-axis image ===
		file_name[index_of_identifier] = 'f';
		if(!load_single_image(file_name,&raw_data_pos_y,&raw_data_p_y_length))
			return 0;
		//=== Loading the negative y-axis image ===
		file_name[index_of_identifier] = 'b';
		if(!load_single_image(file_name,&raw_data_neg_y,&raw_data_n_y_length))
			return 0;
		//=== Loading the positive z-axis image ===
		file_name[index_of_identifier] = 'u';
		if(!load_single_image(file_name,&raw_data_pos_z,&raw_data_p_z_length))
			return 0;
		//=== Loading the negative z-axis image ===
		file_name[index_of_identifier] = 'd';
		if(!load_single_image(file_name,&raw_data_neg_z,&raw_data_n_z_length))
			return 0;

		width = height = w;
		LOGI("cubemap load finished");
		return 1;
	}
	void unload()
	{
		LOGI("cubemap unload started");
		if(raw_data_pos_x)
			free((void*)raw_data_pos_x);
		raw_data_p_x_length = 0;

		if(raw_data_neg_x)
			free((void*)raw_data_neg_x);
		raw_data_n_x_length = 0;

		if(raw_data_pos_y)
			free((void*)raw_data_pos_y);
		raw_data_p_y_length = 0;

		if(raw_data_neg_y)
			free((void*)raw_data_neg_y);
		raw_data_n_y_length = 0;

		if(raw_data_pos_z)
			free((void*)raw_data_pos_z);
		raw_data_p_z_length = 0;

		if(raw_data_neg_z)
			free((void*)raw_data_neg_z);
		raw_data_n_z_length = 0;


		width = height = 0;
		LOGI("cubemap unload finished");
	}

	int init_gl()
	{
		LOGI("cubemap init_gl started");
		GLuint tex_id;
		glGenTextures(1, &tex_id);

		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

		//Binding the 6 face images
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, ETC1_RGB8, width, height, 0, raw_data_p_x_length, (const void *) raw_data_pos_x);
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, ETC1_RGB8, width, height, 0, raw_data_n_x_length, (const void *) raw_data_neg_x);
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, ETC1_RGB8, width, height, 0, raw_data_p_y_length, (const void *) raw_data_pos_y);
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, ETC1_RGB8, width, height, 0, raw_data_n_y_length, (const void *) raw_data_neg_y);
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, ETC1_RGB8, width, height, 0, raw_data_p_z_length, (const void *) raw_data_pos_z);
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, ETC1_RGB8, width, height, 0, raw_data_n_z_length, (const void *) raw_data_neg_z);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//GLES 3.0+ only, not sure what the behavior is here.
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		gl_id = tex_id;
		LOGI("cubemap init_gl finished");
		return 1;
	}

	void term_gl()
	{
		LOGI("cubemap term_gl started");
		if(gl_id)
		{
			glDeleteTextures(1, &gl_id);
		}
		gl_id = 0;
		LOGI("cubemap term_gl finished");
	}
};

#endif //ENGINE_CUBE_MAP_H

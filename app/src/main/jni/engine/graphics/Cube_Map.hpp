//
// Created by F1 on 6/30/2016.
//

#ifndef ENGINE_CUBE_MAP_H
#define ENGINE_CUBE_MAP_H

#include "../common.hpp"
#include "../File_Utils.hpp"


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

	int init_gl();

	void term_gl();

	Cube_Map(const char* filepath,int w);
	~Cube_Map();

private:
	int load_single_image(const char* filepath, const char** data, int* len);
	int load(const char* filepath,int w);
	void unload();
};

#endif //ENGINE_CUBE_MAP_H

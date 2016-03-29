//
// Created by F1 on 3/27/2016.
//

#ifndef ENGINE_FILE_UTILS_H
#define ENGINE_FILE_UTILS_H

#include "common.h"

class File_Utils
{
public:
	static AAssetManager* asset_mgr;
	static const char* read_file_to_string(const char* file_path);
};

#endif //ENGINE_FILE_UTILS_H

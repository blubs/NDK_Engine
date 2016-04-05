//
// Created by F1 on 3/27/2016.
//

#ifndef ENGINE_FILE_UTILS_H
#define ENGINE_FILE_UTILS_H

#include "common.h"

#define SAVE_FILE_PATH "data/data/engine.games.luisg.engine/files/"

class File_Utils
{
public:
	static AAssetManager* asset_mgr;
	static const char* load_asset(const char* file_path);

	static const char* read_savedata(const char* file);
	static void write_savedata(const char* file);
};

#endif //ENGINE_FILE_UTILS_H

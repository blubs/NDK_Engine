//
// Created by F1 on 3/27/2016.
//

#ifndef ENGINE_FILE_UTILS_H
#define ENGINE_FILE_UTILS_H

struct Asset_File
{
	const char* data;
	int len;
};

#include "common.hpp"

#define SAVE_FILE_PATH "data/data/engine.games.luisg.engine/files/"

class File_Utils
{
public:
	static AAssetManager* asset_mgr;
	//Loads a raw asset returning a pointer to the data
	static const char* load_raw_asset(const char* file_path);
	//Loads an asset into a Asset_File struct, includes pointer to the data and file length
	static const Asset_File* load_asset(const char* file_path);

	static const char* read_savedata(const char* file);
	static void write_savedata(const char* file);
};

#endif //ENGINE_FILE_UTILS_H

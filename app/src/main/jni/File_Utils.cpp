//
// Created by F1 on 3/27/2016.
//

#include "File_Utils.h"

const char* File_Utils::read_file_to_buffer (const char *file_path)
{
	AAsset* asset = AAssetManager_open(File_Utils::asset_mgr, file_path, AASSET_MODE_BUFFER);

#ifdef DEBUG_MODE
	if(!File_Utils::asset_mgr)
	{
		LOGE("ERROR: File_Utils::Asset Manager is null, tried reading the file \"%s\" to string.\n",file_path);
	}
#endif

	if(asset == NULL)
	{
		LOGE("ERROR: Failed to read file to string, asset not found. (file: %s)\n",file_path);
		return "";
	}

	const char* asset_buffer = (char*)AAsset_getBuffer(asset);
	off_t file_size = AAsset_getLength(asset);

	//LOGW("File_read_to_buffer: %s\n",asset_buffer);
	//Allocating the file data.
	char* alloced_data = (char*)malloc(sizeof(char) * file_size);
	//How and when am I going to clear this array that is allocated?
	memcpy(alloced_data,asset_buffer,(size_t)file_size);
	AAsset_close(asset);
	return alloced_data;
}



AAssetManager* File_Utils::asset_mgr = NULL;

//Alternative way of handling file loading
//May or may not allow the writing of files
//This requires the permission:
//<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
//inside of AndroidManifest.xml, enable it if we use this method.
/*FILE* fp = fopen("test_text.txt","r");
if(fp == NULL)
	LOGW("file found is null");
else
	LOGW("file found is not null");

if(fp != NULL)
	fclose(fp);*/


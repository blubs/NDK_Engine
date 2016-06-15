//
// Created by F1 on 3/27/2016.
//

#include "File_Utils.h"

const char* File_Utils::load_raw_asset (const char *file_path)
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
		return NULL;
	}

	const char* asset_buffer = (char*)AAsset_getBuffer(asset);
	off_t file_size = AAsset_getLength(asset);

	//LOGW("File_read_to_buffer: %s\n",asset_buffer);
	//Allocating the file data.
	char* alloced_data = (char*)malloc(sizeof(char) * file_size);
	//How and when am I going to clear this array that is allocated?
	memcpy(alloced_data,asset_buffer,(size_t)file_size);
	AAsset_close(asset);

	//if(file_path[0] == 's' && (file_path[1] == 'k' || file_path[1] == 't'))
	if(file_path[0] == 'm' && (file_path[1] == 'o' || file_path[1] == 'i'))
	{
		//temporarily print the array for debug.
		LOGE("========= Shader %s ============",file_path);
		char* temp_shader = (char*) malloc(sizeof(char) * file_size);
		int index = 0;
		for(int i = 0; i < file_size; i++)
		{
			temp_shader[index] = alloced_data[i];
			//if(temp_shader[index] == '\n')
			//{
			//	temp_shader[index] = '\0';
			//	LOGE("src:%s",temp_shader);
			//	temp_shader[index] = '\n';
			//	index = 0;
			//}
			//else if(temp_shader[index] == '\0')
			//{
			//	LOGE("src:%s",temp_shader);
			//	index = 0;
			//}
			LOGE("char: %c (%d)",temp_shader[i],temp_shader[i]);
			index++;
		}
		free(temp_shader);
	}


	return alloced_data;
}

const Asset_File* File_Utils::load_asset (const char *file_path)
{
	Asset_File* f = (Asset_File*)malloc(sizeof(Asset_File));
	AAsset* asset = AAssetManager_open(File_Utils::asset_mgr, file_path, AASSET_MODE_BUFFER);

	f->data = NULL;
	f->len = 0;

#ifdef DEBUG_MODE
	if(!File_Utils::asset_mgr)
	{
		LOGE("ERROR: File_Utils::Asset Manager is null, tried reading the file \"%s\" to string.\n",file_path);
	}
#endif

	if(asset == NULL)
	{
		LOGE("ERROR: Failed to read file to string, asset not found. (file: %s)\n",file_path);
		return f;
	}

	const char* asset_buffer = (char*)AAsset_getBuffer(asset);
	off_t file_size = AAsset_getLength(asset);

	//LOGW("File_read_to_buffer: %s\n",asset_buffer);
	//Allocating the file data.
	char* alloced_data = (char*)malloc(sizeof(char) * file_size);
	//How and when am I going to clear this array that is allocated?
	memcpy(alloced_data,asset_buffer,(size_t)file_size);
	AAsset_close(asset);
	f->data = alloced_data;
	f->len = file_size;
	return f;
}



const char* File_Utils::read_savedata(const char* file_name)
{
	//test.dat
	int len = strlen(SAVE_FILE_PATH) + strlen(file_name);

	char* file_path = (char*)malloc(sizeof(char)*len);
	strcpy(file_path,SAVE_FILE_PATH);
	strcat(file_path,file_name);

	//Open the file for reading
	FILE* f = fopen(file_path,"r");
	if(!f)
	{
		LOGE("Error: opening file \"%s\" for read failed.\n",file_name);
		return NULL;
	}

	char* data = (char*)malloc(sizeof(char)*100);//TODO: replace 100 with an expected data size.
	fgets(data,100,f);//TODO: also replace this 100

#ifdef DEBUG_MODE
	LOGI("fopen file \"%s\":%s\n",file_name,data);
#endif
	fclose(f);

	return data;
}
void File_Utils::write_savedata(const char* file_name)
{
	//test.dat
	//TODO: what data are we going to store?
	//TODO: what parameters does this function require to store?
	//TODO: what data are we going to store?
	//TODO: need a helper function that transforms this data to the exact format that we're going to store it in.
	int len = strlen(SAVE_FILE_PATH) + strlen(file_name);

	char* file_path = (char*)malloc(sizeof(char)*len);
	strcpy(file_path,SAVE_FILE_PATH);
	strcat(file_path,file_name);

	//Open the file for writing (currently for appending)
	FILE* f = fopen(file_path,"a+");//w for write, a+ for append (used to test/ensure lifetime of stored data)
	if(!f)
	{
		LOGE("Error: opening file \"%s\" for write failed.\n",file_name);
		return;
	}

#ifdef DEBUG_MODE
	LOGI("writing to fopen file \"%s\".\n",file_name);
#endif

	fputs("test",f);//TODO: replace "test" with the transformed data buffer
	fclose(f);

	free(file_path);
}

AAssetManager* File_Utils::asset_mgr = NULL;

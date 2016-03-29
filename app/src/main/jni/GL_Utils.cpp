//
// Created by F1 on 3/28/2016.
//

#include "GL_Utils.h"

GLuint GL_Utils::load_shader(const char* shader_path, GLenum type)
{
	const char* shader_src = File_Utils::read_file_to_string(shader_path);

	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);

	if(!shader)
	{
		LOGE("Error: Shader could not be created (\"%s\", type:%d)\n",shader_path,type);
		return 0;
	}

	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE)
	{
		LOGE("Error: Failed to compile shader \"%s\".\n",shader_path);

		GLint log_size = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

		GLchar* info_log = (GLchar*)malloc(sizeof(GLchar)*log_size);
		glGetShaderInfoLog(shader, log_size, &log_size, info_log);
		LOGE("   compilation log: %s.\n",info_log);
		free(info_log);

		glDeleteShader(shader);
		return 0;
	}
	free((void*)shader_src);
	return shader;
}

void GL_Utils::unload_shader(GLuint shader)
{
	glDeleteShader(shader);
}

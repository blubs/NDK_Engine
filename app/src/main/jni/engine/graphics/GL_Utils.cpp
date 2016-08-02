//
// Created by F1 on 3/28/2016.
//

#include "GL_Utils.hpp"

GLuint GL_Utils::load_shader(const char* shader_src, const char* shader_path, GLenum type)
{
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
	};
	return shader;
}

void GL_Utils::unload_shader(GLuint shader)
{
	glDeleteShader(shader);
}


// Converts index 0-31 to GL_TEXTURE0-GL_TEXTURE31
// 	because opengl uses enums for this which doesn't allow us to index into an arbitrary one
GLenum GL_Utils::tex_index_to_enum(int i)
{
	switch(i)
	{
		case 0:
			return GL_TEXTURE0;
		case 1:
			return GL_TEXTURE1;
		case 2:
			return GL_TEXTURE2;
		case 3:
			return GL_TEXTURE3;
		case 4:
			return GL_TEXTURE4;
		case 5:
			return GL_TEXTURE5;
		case 6:
			return GL_TEXTURE6;
		case 7:
			return GL_TEXTURE7;
		case 8:
			return GL_TEXTURE8;
		case 9:
			return GL_TEXTURE9;
		case 10:
			return GL_TEXTURE10;
		case 11:
			return GL_TEXTURE11;
		case 12:
			return GL_TEXTURE12;
		case 13:
			return GL_TEXTURE13;
		case 14:
			return GL_TEXTURE14;
		case 15:
			return GL_TEXTURE15;
		case 16:
			return GL_TEXTURE16;
		case 17:
			return GL_TEXTURE17;
		case 18:
			return GL_TEXTURE18;
		case 19:
			return GL_TEXTURE19;
		case 20:
			return GL_TEXTURE20;
		case 21:
			return GL_TEXTURE21;
		case 22:
			return GL_TEXTURE22;
		case 23:
			return GL_TEXTURE23;
		case 24:
			return GL_TEXTURE24;
		case 25:
			return GL_TEXTURE25;
		case 26:
			return GL_TEXTURE26;
		case 27:
			return GL_TEXTURE27;
		case 28:
			return GL_TEXTURE28;
		case 29:
			return GL_TEXTURE29;
		case 30:
			return GL_TEXTURE30;
		case 31:
			return GL_TEXTURE31;
		default:
			break;
	}
	LOGE("Error: tried loading out of bounds texture %d\n",i);
	return GL_TEXTURE0;
}

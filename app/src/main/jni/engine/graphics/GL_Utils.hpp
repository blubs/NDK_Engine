//
// Created by F1 on 3/28/2016.
//

#ifndef ENGINE_GL_UTILS_H
#define ENGINE_GL_UTILS_H

#include "../common.hpp"

class GL_Utils
{
public:
	static GLuint load_shader (const char *shader_src, const char *shader_path, GLenum type);

	static void unload_shader (GLuint shader);

	static GLenum tex_index_to_enum(int i);
};

#endif //ENGINE_GL_UTILS_H

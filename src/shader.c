#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include "glad/glad.h"
#include "file.h"

shader_t shader_create(const char *path_vertex, const char *path_fragment) {
	const char *paths[2] = { path_vertex, path_fragment };
	const uint32_t enums[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, };
	char *sources[2];
	uint32_t shaders[2];
	uint32_t shader;

	shader = glCreateProgram();
	for(uint8_t i = 0; i < 2; i++) {
		shaders[i] = glCreateShader(enums[i]);
		sources[i] = file_load_contents(paths[i]);
		glShaderSource(shaders[i], 1, (const char * const *)&sources[i], NULL);
		glCompileShader(shaders[i]);

		{
			int32_t success;
			char info_log[512];
			glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(shaders[i], sizeof(info_log), NULL, info_log);
				fprintf(stderr, "ERROR: Shader fucked up: %s\n", info_log);
				return 1;
			}
		}

		glAttachShader(shader, shaders[i]);
		free(sources[i]);
		glDeleteShader(shaders[i]);
	}
	glLinkProgram(shader);

	return shader;
}

void shader_destroy(const shader_t shader) {
	glDeleteProgram(shader);
}

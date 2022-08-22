#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

texture_t texture_create(const char *path, const int32_t wrap_mode, const int32_t min_filter, const int32_t mag_filter, const uint8_t mipmap) {
	int32_t color_formats[5] = {
		0,
		GL_RED,
		GL_RG,
		GL_RGB,
		GL_RGBA
	};

	texture_t t;
	GLubyte *data;
	glGenTextures(1, &t.id);
	glBindTexture(GL_TEXTURE_2D, t.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	stbi_set_flip_vertically_on_load(1);
	data = stbi_load(path, &t.width, &t.height, &t.channels, 0);
	if(!data) {
		printf("ERROR: Texture fucked up: %s\n", path);
		glfwTerminate();
		return t;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, color_formats[t.channels], t.width, t.height, 0, (uint32_t)color_formats[t.channels], GL_UNSIGNED_BYTE, data);

	if(mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	return t;
}

void texture_destroy(texture_t *t) {
	glDeleteTextures(1, &t->id);
}

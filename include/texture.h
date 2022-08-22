#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
	uint32_t id;
	int32_t width, height, channels;
} texture_t;

texture_t texture_create(const char *path, const int32_t wrap_mode, const int32_t min_filter, const int32_t mag_filter, const uint8_t mipmap);
void texture_destroy(texture_t *t);

#endif

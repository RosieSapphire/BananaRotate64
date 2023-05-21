#ifndef SPRITE_H
#define SPRITE_H

#include <cglm/cglm.h>
#include "texture.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

typedef struct {
	texture_t* textures;
	uint16_t texture_count;
	uint16_t padding1;
	uint32_t padding2;
} sprite_t;

void sprite_init_buffers(void);
void sprite_create(sprite_t *sprite, const uint16_t texture_count, const char **texture_paths);
void sprite_draw(sprite_t sprite, const uint16_t texture_index, const float angle, const float scale, const vec2 pos);
void sprite_destroy(sprite_t *sprite);
void sprite_delete_buffers(void);

#endif

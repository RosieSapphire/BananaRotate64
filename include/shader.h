#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>

typedef uint32_t shader_t;
shader_t shader_create(const char *path_vertex, const char *path_fragment);
void shader_destroy(const shader_t shader);

#endif

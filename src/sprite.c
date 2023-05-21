#include "sprite.h"

#include <stdlib.h>
#include <cglm/cglm.h>

#include "glad/glad.h"
#include "shader.h"

static const float vertices[] = {
	 0.5f, -0.5f,	1.0f, 1.0f,
	-0.5f,  0.5f,	0.0f, 0.0f,
	-0.5f, -0.5f,	0.0f, 1.0f,
	 0.5f,  0.5f,	1.0f, 0.0f,
};

static const uint8_t indices[] = {
	0, 2, 1,
	0, 1, 3
};

static uint32_t sprite_vao;
static uint32_t sprite_vbo;
static uint32_t sprite_ebo;
static shader_t sprite_shader;

void sprite_init_buffers() {
	mat4 matrix_projection;
	glm_ortho(0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, matrix_projection);

	glGenVertexArrays(1, &sprite_vao);
	glBindVertexArray(sprite_vao);

	glGenBuffers(1, &sprite_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &sprite_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	sprite_shader = shader_create("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
	glUseProgram(sprite_shader);
	glUniformMatrix4fv(glGetUniformLocation(sprite_shader, "projection"), 1, GL_FALSE, (const float *)matrix_projection);
}

void sprite_create(sprite_t *sprite, const uint16_t texture_count, const char **texture_paths) {
	uint8_t i;
	sprite->textures = calloc(texture_count, sizeof(texture_t));
	sprite->texture_count = texture_count;
	for(i = 0; i < texture_count; i++) {
		sprite->textures[i] = texture_create(texture_paths[i], GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1);
	}
}

void sprite_draw(sprite_t sprite, const uint16_t texture_index, const float angle, const float scale, const vec2 pos) {
	mat4 transform;
	glm_mat4_copy(GLM_MAT4_IDENTITY, transform);
	glm_translate(transform, (vec3){pos[0], pos[1], 0.0f});
	glm_scale(transform, (vec3){(WINDOW_WIDTH / 2.0f) * scale, (WINDOW_HEIGHT / 2.0f) * scale, 0.0f});
	glm_rotate(transform, angle, GLM_ZUP);

	glUniformMatrix4fv(glGetUniformLocation(sprite_shader, "matrix"), 1, GL_FALSE, (const float *)transform);

	glBindTexture(GL_TEXTURE_2D, sprite.textures[texture_index].id);
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_BYTE, indices);
}

void sprite_destroy(sprite_t *sprite) {
	uint8_t i;
	for(i = 0; i < sprite->texture_count; i++) {
		texture_destroy(&sprite->textures[i]);
	}
}

void sprite_delete_buffers() {
	shader_destroy(sprite_shader);
	glDeleteVertexArrays(1, &sprite_vao);
	glDeleteBuffers(1, &sprite_vbo);
	glDeleteBuffers(1, &sprite_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

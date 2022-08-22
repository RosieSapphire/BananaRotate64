#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.h"
#include "file.h"

#define WINDOW_TITLE 	"Banana Rotate"
#define WINDOW_WIDTH 	1024
#define WINDOW_HEIGHT	1024

static float banana_vertices[] = {
	 0.5f, -0.5f,	1.0f, 0.0f,
	-0.5f,  0.5f,	0.0f, 1.0f,
	-0.5f, -0.5f,	0.0f, 0.0f,
	 0.5f,  0.5f,	1.0f, 1.0f,
};

static uint8_t banana_indices[] = {
	0, 1, 2,
	0, 3, 1
};

static GLFWwindow *window;
static double time_now;
static double time_last;

static uint32_t banana_vao;
static uint32_t banana_vbo;
static uint32_t banana_ebo;
static uint32_t banana_shader;
static texture_t banana_texture;
static float banana_speed = 1.0f;

static float banana_rotation_angle = 0.0f;
static uint64_t banana_rotation_count = 0;

static texture_t button_texture;
static uint8_t button_holding;

static mat4 matrix_model;
static mat4 matrix_projection;

static uint8_t mouse_first_press = 0;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	{
		const char *banana_shader_paths[2] = { "res/shaders/vertex.glsl", "res/shaders/fragment.glsl", };
		const uint32_t shader_enums[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, };
		char *banana_shader_sources[2];
		uint32_t banana_shaders[2];

		banana_shader = glCreateProgram();
		for(uint8_t i = 0; i < 2; i++) {
			banana_shaders[i] = glCreateShader(shader_enums[i]);
			banana_shader_sources[i] = file_load_contents(banana_shader_paths[i]);
			glShaderSource(banana_shaders[i], 1, (const char * const *)&banana_shader_sources[i], NULL);
			glCompileShader(banana_shaders[i]);

			{
				int32_t success;
				char info_log[512];
				glGetShaderiv(banana_shaders[i], GL_COMPILE_STATUS, &success);
				if(!success) {
					glGetShaderInfoLog(banana_shaders[i], sizeof(info_log), NULL, info_log);
					fprintf(stderr, "ERROR: Shader fucked up: %s\n", info_log);
					return 1;
				}
			}

			glAttachShader(banana_shader, banana_shaders[i]);
			free(banana_shader_sources[i]);
			glDeleteShader(banana_shaders[i]);
		}
		glLinkProgram(banana_shader);
	}

	banana_texture = texture_create("res/textures/banana.png", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1);
	button_texture = texture_create("res/textures/button.png", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1);

	glGenVertexArrays(1, &banana_vao);
	glBindVertexArray(banana_vao);

	glGenBuffers(1, &banana_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, banana_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(banana_vertices), banana_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &banana_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, banana_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(banana_indices), banana_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glUseProgram(banana_shader);
	glUniform1i(glGetUniformLocation(banana_shader, "tex"), 0);

	glm_ortho(0.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, -1.0f, 1.0f, matrix_projection);

	time_now = glfwGetTime();
	time_last = time_now;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while(!glfwWindowShouldClose(window)) {
		double mouse_pos[2];
		float time_delta;
		time_last = time_now;
		time_now = glfwGetTime();
		time_delta = (float)(time_now - time_last);

		button_holding = 0;
		glfwGetCursorPos(window, &mouse_pos[0], &mouse_pos[1]);
		switch(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			case GLFW_PRESS: {
				const uint8_t mouse_over_button = glm_vec2_distance2((vec2){(float)mouse_pos[0], (float)mouse_pos[1]}, (vec2){922.0f, 922.0f}) < (WINDOW_WIDTH * 0.05f) * (WINDOW_WIDTH * 0.05f);
				if(!mouse_first_press && mouse_over_button) {
					banana_speed *= 1.01f;
					mouse_first_press = 1;
				}
				button_holding = mouse_over_button;
				break;
			}

			case GLFW_RELEASE:
				mouse_first_press = 0;
				break;
		}

		banana_rotation_angle += banana_speed * time_delta * GLM_PIf;
		if(banana_rotation_angle > 2 * GLM_PIf) {
			banana_rotation_count++;
			// printf("Banana rotated %ld times.\tSpeed: %.2fRPM\n", banana_rotation_count, (double)banana_speed * 30.0);
			banana_rotation_angle = 0.0f;
		}
		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_model);
		glm_translate(matrix_model, (vec3){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f});
		glm_scale(matrix_model, (vec3){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f});
		glm_rotate(matrix_model, banana_rotation_angle, GLM_ZUP);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniformMatrix4fv(glGetUniformLocation(banana_shader, "projection"), 1, GL_FALSE, (const float *)matrix_projection);
		glUniformMatrix4fv(glGetUniformLocation(banana_shader, "matrix"), 1, GL_FALSE, (const float *)matrix_model);
		glBindTexture(GL_TEXTURE_2D, banana_texture.id);
		glDrawElements(GL_TRIANGLES, sizeof(banana_indices), GL_UNSIGNED_BYTE, banana_indices);

		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_model);
		glm_translate(matrix_model, (vec3){WINDOW_WIDTH - (WINDOW_WIDTH * 0.1f), WINDOW_HEIGHT - (WINDOW_HEIGHT * 0.1f), 0.0f});
		glm_scale(matrix_model, (vec3){WINDOW_WIDTH * 0.1f, WINDOW_HEIGHT * 0.1f, 0.0f});
		glm_rotate(matrix_model, glm_rad(180.0f), GLM_XUP);
		glm_scale(matrix_model, (vec3){1.0f - ((float)(button_holding) * 0.1f), 1.0f - ((float)(button_holding) * 0.1f), 0.0f});
		glUniformMatrix4fv(glGetUniformLocation(banana_shader, "matrix"), 1, GL_FALSE, (const float *)matrix_model);
		glBindTexture(GL_TEXTURE_2D, button_texture.id);
		glDrawElements(GL_TRIANGLES, sizeof(banana_indices), GL_UNSIGNED_BYTE, banana_indices);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	texture_destroy(&button_texture);
	texture_destroy(&banana_texture);
	glDeleteProgram(banana_shader);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "sprite.h"
#include "texture.h"
#include "file.h"

static GLFWwindow *window;

static double time_now;
static double time_last;

static sprite_t banana_sprite;
static float banana_speed = 1.0f;
static float banana_rotation_angle = 0.0f;
static uint64_t banana_rotation_count = 0;

static sprite_t button_sprite;
static uint8_t button_holding;
static uint8_t button_first_press = 0;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Banana Rotate", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	sprite_init_buffers();
	{
		const char *banana_sprite_path = "res/textures/banana.png";
		const char *button_sprite_path = "res/textures/button.png";
		sprite_create(&banana_sprite, 1, &banana_sprite_path);
		sprite_create(&button_sprite, 1, &button_sprite_path);
	}

	time_now = glfwGetTime();
	time_last = time_now;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

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
				vec2 mouse_pos_float = {(float)mouse_pos[0], (float)mouse_pos[1]};
				const uint8_t mouse_over_button = glm_vec2_distance2(mouse_pos_float, (vec2){922.0f, 922.0f}) < (WINDOW_WIDTH * 0.05f) * (WINDOW_WIDTH * 0.05f);
				if(!button_first_press && mouse_over_button) {
					banana_speed *= 1.01f;
					button_first_press = 1;
				}
				button_holding = mouse_over_button;
				break;
			}

			case GLFW_RELEASE:
				button_first_press = 0;
				break;
		}

		banana_rotation_angle += banana_speed * time_delta * GLM_PIf;
		if(banana_rotation_angle > 2 * GLM_PIf) {
			banana_rotation_count++;
			banana_rotation_angle = 0.0f;
			printf("Banana rotated %ld times.\tSpeed: %.2fRPM\n", banana_rotation_count, (double)banana_speed * 30.0);
		}

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		sprite_draw(banana_sprite, 0, banana_rotation_angle, 1.0f, (vec2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});
		sprite_draw(button_sprite, 0, 0.0f, (1.0f - ((float)(button_holding) * 0.1f)) * 0.2f, (vec2){WINDOW_WIDTH - (WINDOW_WIDTH * 0.1f), WINDOW_HEIGHT - (WINDOW_HEIGHT * 0.1f)});

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	sprite_destroy(&button_sprite);
	sprite_destroy(&banana_sprite);
	sprite_delete_buffers();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

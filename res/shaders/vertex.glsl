#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

out vec2 uv;

uniform mat4 projection;
uniform mat4 matrix;

void main() {
	gl_Position = projection * matrix * vec4(a_pos, 0.0f, 1.0f);
	uv = a_uv;
}

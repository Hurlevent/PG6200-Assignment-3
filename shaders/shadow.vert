#version 330

uniform mat4 light_transform;
//uniform mat4 modelviewprojection_matrix;

in vec3 position;
//in vec3 normal;


void main() {
	gl_Position = light_transform * vec4(position, 1.0);
}
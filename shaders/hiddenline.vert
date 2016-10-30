#version 330

uniform vec4 projection_matrix;
uniform vec4 modelview_matrix;
uniform vec3 normal_matrix;
uniform vec3 light_pos;

in vec3 position;
in vec3 normal;

smooth out vec3 g_v;
smooth out vec3 g_l;
smooth out vec3 g_n;

void main(){
	vec4 pos = modelview_matrix * vec4(position, 1.0);

	g_v = normalize(-pos.xyz);
	g_l = normalize(light_pos - pos.xyz);
	g_n = normal_matrix * normal;

	gl_Position = projection_matrix * pos;
}
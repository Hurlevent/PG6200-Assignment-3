#version 330
 

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform vec3 color;
//uniform mat4 rotation_matrix;

smooth in vec3 g_n[3];
smooth in vec3 g_v[3];
smooth in vec3 g_l[3];

smooth out vec3 f_n;
smooth out vec3 f_v;
smooth out vec3 f_l;

flat out vec3 f_c;

void main(){
	float sum_of_x = g_v[0].x + g_v[1].x + g_v[2].x;
	float sum_of_y = g_v[0].y + g_v[1].y + g_v[2].y;
	float sum_of_z = g_v[0].z + g_v[1].z + g_v[2].z;

	vec3 cog = vec3(sum_of_x / 3.0f, sum_of_y / 3.0f, sum_of_z / 3.0f);

	for(int i = 0; i < gl_in.length(); i++){
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];

		f_c = color;

		vec4 v0 = vec4(normalize(cog - f_v), 1.0f);

		gl_Position = gl_in[i].gl_Position + 0.02*v0;
		EmitVertex();
	}
	EndPrimitive();



	for(int i = 0; i < gl_in.length(); i++){
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];

		f_c = vec3(0.0);

		mat4 reposition_matrix = mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.1f, 1.0));

		gl_Position = reposition_matrix * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}

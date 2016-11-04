#version 330
 

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

//uniform vec3 hiddenline_color;

smooth in vec3 g_n[3];
smooth in vec3 g_v[3];
smooth in vec3 g_l[3];

in vec4 crd[3];
in vec3 cube_map_coord[3];


out vec4 texture_coord;
out vec3 diffuse_cube_coord;

smooth out vec3 f_n;
smooth out vec3 f_v;
smooth out vec3 f_l;

smooth out vec3 bary;

void main(){
	bary = vec3(1.0, 0.0, 0.0);

	gl_Position = gl_in[0].gl_Position;

	f_n = g_n[0];
	f_v = g_v[0];
	f_l = g_l[0];

	texture_coord = crd[0];
	diffuse_cube_coord = cube_map_coord[0];

	EmitVertex();

	bary = vec3(0.0, 1.0, 0.0);

	gl_Position = gl_in[1].gl_Position;

	f_n = g_n[1];
	f_v = g_v[1];
	f_l = g_l[1];

	texture_coord = crd[1];
	diffuse_cube_coord = cube_map_coord[1];

	
	EmitVertex();

	bary = vec3(0.0, 0.0, 1.0);

	gl_Position = gl_in[2].gl_Position;

	f_n = g_n[2];
	f_v = g_v[2];
	f_l = g_l[2];

	texture_coord = crd[2];
	diffuse_cube_coord = cube_map_coord[2];

	
	EmitVertex();

	EndPrimitive();
}

/*
void main(){
	float sum_of_x = g_v[0].x + g_v[1].x + g_v[2].x;
	float sum_of_y = g_v[0].y + g_v[1].y + g_v[2].y;
	float sum_of_z = g_v[0].z + g_v[1].z + g_v[2].z;

	vec3 cog = vec3(sum_of_x / 3.0f, sum_of_y / 3.0f, sum_of_z / 3.0f);

	for(int i = 0; i < gl_in.length(); i++){
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];

		f_c = hiddenline_color;

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
*/

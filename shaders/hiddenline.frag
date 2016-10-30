#version 150

//uniform vec3 color;


smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
flat in vec3 f_c;

out vec4 out_color;


void main(){

	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);

    out_color = vec4(diff*f_c + spec, 1.0);
}


/*
smooth in vec3 bary;

// Creates an anti-aliased line
float amplify(float d, float scale, float offset){
	d = scale * d + offset;
	d = clamp(d, 0, 1);
	d = 1 - exp2(-2*d*d);
	return d;
}

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	
	vec3 phong_color = diff*color + spec;

	float min_bary = 0.1f;
	
	phong_color = amplify(min_bary, 40, -0.5)*phong_color;

    out_color = vec4(phong_color, 1.0);

	
}

*/
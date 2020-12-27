#define PI 3.14159265359

//inputs from vertex shader
in vec4 vert_pos;
in vec4 color;

//uniform values
uniform sampler2D texture;
uniform float resolution;

//sample from the 1D distance map
float sample(vec2 coord, float r) {
	return step(r, texture2D(texture, coord).r);
}

void main(void) {
	//rectangular to polar
	vec2 norm = vert_pos.st;
	float theta = atan(norm.y, norm.x);
	float r = length(norm);	
	float coord = (theta + PI) / (2.0 * PI);
	
	//the tex coord to sample our 1D lookup texture	
	//always 0.0 on y axis
	vec2 tc = vec2(coord, 0.0);
	
	//the center tex coord, which gives us hard shadows
	float center = sample(tc, r);
	
	//we multiply the blur amount by our distance from center
	//this leads to more blurriness as the shadow "fades away"
	float blur = (1.0 / resolution)  * smoothstep(0.0, 1.0, r); 
	
	//now we use a simple gaussian blur
	float sum = 0.0;
	
	sum += sample(vec2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
	sum += sample(vec2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
	sum += sample(vec2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
	sum += sample(vec2(tc.x - 1.0 * blur, tc.y), r) * 0.15;
	
	sum += center * 0.16;
	
	sum += sample(vec2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
	sum += sample(vec2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
	sum += sample(vec2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
	sum += sample(vec2(tc.x + 4.0 * blur, tc.y), r) * 0.01;
	
	//sum of 1.0 -> in light, 0.0 -> in shadow
 	
 	//multiply the summed amount by our distance, which gives us a radial falloff
 	//then multiply by vertex (light) color
 	gl_FragColor = color * vec4(vec3(1.0), sum * smoothstep(1.0, 0.0, r));
}

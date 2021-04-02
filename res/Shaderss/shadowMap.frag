#define PI 3.14159265359

//inputs from vertex shader
in vec4 vert_pos;

//uniform values
uniform sampler2D texture;
uniform float resolution;

//alpha threshold for our occlusion map
const float THRESHOLD = 0.75;

void main(void) {
	float distance = 1.0;
  
	for (float i = 0.0; i < resolution; i += 1.0) {
		//rectangular to polar filter
		float normLen = (i / resolution) * 2.0 - 1.0;
		float theta = 0.5 * PI - (vert_pos.s + 1.0) * PI; 
		float r = i / resolution;
		
		//coord which we will sample from occlude map
		vec2 coord = vec2(-r * sin(theta), -r * cos(theta)) / 2.0 + 0.5;
		
		//sample the occlusion map
		vec4 pixel = texture2D(texture, coord);
		
		//the current distance is how far from the top we've come
		float dst = i / resolution;
		
		//if we've hit an opaque fragment (occluder), then get new distance
		//if the new distance is below the current, then we'll use that for our ray
		if (pixel.a > THRESHOLD) {
			distance = min(distance, dst);
		}
	} 
	gl_FragColor = vec4(vec3(distance), 1.0);
}
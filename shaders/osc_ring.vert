#version 150

uniform samplerBuffer t_left;

uniform float output_size_1; // 1/output_size
uniform float scale;
uniform float rms;


const float PI2 = radians(360.);
void main(){
	// calculate x coordinate
	//float x = mix(-1., 1., (float(gl_VertexID) + 0.5) * output_size_1);
	//gl_Position = vec4(x, clamp(y * scale_new, -1.0, 1.0), 0.0, 1.0);


	float value = texelFetch(t_left, gl_VertexID).x + 0.5; 
	float scale_new = mix(scale * 2., scale/2., rms);
	float t = (float(gl_VertexID) + 0.5) * output_size_1 * PI2;
	vec2 cpos = vec2(cos(t), sin(t)) * 0.5;
	gl_Position = vec4(cpos * value * scale_new, 0.0, 1.0);
}

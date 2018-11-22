#version 150

uniform samplerBuffer t_left;

uniform float output_size_1; // 1/output_size
uniform float scale;
uniform float rms;

void main(){
	// calculate x coordinate
	float x = mix(-1., 1., (float(gl_VertexID) + 0.5) * output_size_1);
	float y = texelFetch(t_left, gl_VertexID).x;
	float scale_new = mix(scale * 2., scale/4., rms);
	gl_Position = vec4(x, clamp(y * scale_new, -1.0, 1.0), 0.0, 1.0);
}

#version 150

uniform samplerBuffer t_left;

uniform float output_size_1; // 1/output_size
uniform float scale;
uniform float rms;
uniform int buffer_size;

void main(){
	// calculate x coordinate
	float x = mix(-1., 1., (float(gl_VertexID) + 0.5) * output_size_1);

	// linear lookup in buffer
	float index = gl_VertexID * output_size_1 * buffer_size;
	int bot = max(int(floor(index)), 0);
	int top = min(int(ceil(index)), buffer_size-1);
	float ybot = texelFetch(t_left, bot).x;
	float ytop = texelFetch(t_left, top).x;
	float y = mix(ybot, ytop, fract(index));

	// auto scaling
	float scale_new = mix(scale * 1.5, scale/10., rms);
	gl_Position = vec4(x, clamp(y * scale_new, -1.0, 1.0), 0.0, 1.0);
}

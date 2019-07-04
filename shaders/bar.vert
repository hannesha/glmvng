#version 330

uniform samplerBuffer t_f_left;

uniform vec4 bot_color = vec4(1., 0.3, 0.3, 1.);
uniform vec4 top_color = vec4(1., 0.3, 0.3, 1.);

const float upper_limit = -10;
const float lower_limit = -60;

const float slope = 40.0/(upper_limit - lower_limit);
const float offset = 1 - upper_limit/20.0 * slope;

// switch gradient, 0:full range per bar, 1:0dB has top_color
const float gradient = 1.;
uniform float output_size_1;

out vec4 v_bot_color;
out vec4 v_top_color;

void main () {
	float y = texelFetch(t_f_left, gl_VertexID).x *slope +offset;

	// calculate x coordinates
	float x = mix(-1., 1., (float(gl_VertexID) + 0.5) * output_size_1);
	
	gl_Position = vec4(x, y, 0.0, 1.0);
	v_bot_color = bot_color;

	// calculate normalized top color	
	y = mix(1.0, y, gradient);
	v_top_color = mix(bot_color, top_color, y* 0.5 + 0.5);
}

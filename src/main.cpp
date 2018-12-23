#include <iostream>
#include <string>
#include <stdexcept>

#include "GLFW.hpp"
#include "GLXwindow.hpp"

#include "Config.hpp"
#include "Drawbuffer.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Pulse_Async.hpp"
#include "Processing.hpp"
#include "FFT.hpp"

#include <cmath>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void sigint_handler(int signal){
	running = false;
}

int main(int argc, char *argv[]) {
	std::signal(SIGINT, sigint_handler);
	auto window = GLXwindow();

	window.setTitle("window title");

	// load extensions
	GL::init();
	// clear window
	window.pollEvents();
	glClearColor(0.0, 0.0, 0.0, 0.8);
	glClear(GL_COLOR_BUFFER_BIT);
	window.swapBuffers();

	std::string config_file = "config";

	if(argc > 1){
		config_file = argv[1];
	}

	Config cfg(config_file);

	// init buffers
	std::shared_ptr<DrawBuffer> draw_buf = std::make_shared<DrawBuffer>();
	GLDEBUG;
	Renderer rend(cfg.renderers[0], draw_buf);
	GLDEBUG;
	// create audio buffer
	Buffers<float>::Ptr bufs(new Buffers<float>());
	bufs->bufs.emplace_back(cfg.input.buffer_len);
	// init fft
	FFT fft(cfg.input.fft_size);
	std::vector<Magnitudes> mags(1, Magnitudes(cfg.input.fft_size));
	// create processing buffer
	Processing::GravityInfo gravity_info(cfg.input.fft_size);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLDEBUG;
	
	// init audio stream
	Input::Ptr input(new Pulse_Async(bufs));
	input->start_stream(cfg.input);

	// mainloop
	glClearColor(0.0, 0.0, 0.0, 0.8);
	float old_rms = 0.5;
	float rms_mix = 0.8;
	do{
		window.pollEvents();
		// calculate rms
		float rms = std::sqrt(Processing::rms(bufs->bufs[0]) / bufs->bufs[0].size());
		old_rms = old_rms * (1.f - rms_mix) + rms * rms_mix;
		// set rms
		ShaderConfig::value_type vrms = {"rms", Scalar(old_rms)};
		rend.set_uniform(vrms);
		// update buffer
		draw_buf->update(bufs->bufs);

		// calulate spectrum
		fft.calculate(bufs->bufs[0]);
		// processing
		fft.magnitudes(mags[0], 1);
		const float dt = 0.016;
		const float gravity = 1;
		Processing::calculate_gravity(mags[0], gravity_info, gravity, dt);

		// update buffer
		draw_buf->update_fft(mags);

		// draw
		glClear(GL_COLOR_BUFFER_BIT);
		rend.draw();

		window.swapBuffers();
	}
	while(!window.shouldClose() && running);

	return 0;
}

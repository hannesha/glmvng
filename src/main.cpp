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

	// create audio buffer
	Buffers<float>::Ptr bufs(new Buffers<float>());
	bufs->bufs.emplace_back(cfg.input.buffer_len);
	if(cfg.input.stereo){
		bufs->bufs.emplace_back(cfg.input.buffer_len);
	}
	// init buffers
	std::shared_ptr<DrawBuffer> draw_buf = std::make_shared<DrawBuffer>();
	GLDEBUG;
	std::vector<Renderer> renderers;
	for(auto& rconfig : cfg.renderers){
		renderers.emplace_back(rconfig, draw_buf);
		GLDEBUG;
	}

	// init fft
	std::vector<std::tuple<FFT, Processing::GravityInfo>> processing_data;
	processing_data.emplace_back(FFT(cfg.input.fft_size), Processing::GravityInfo(cfg.input.fft_size, {0, 10}));
	std::vector<Magnitudes> mags(1, Magnitudes(cfg.input.fft_size));

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
		for(auto& renderer : renderers){
			renderer.set_uniform(vrms);
		}
		// update buffer
		draw_buf->update(bufs->bufs);

		// calulate spectrum
		const float dt = 0.016;
		const float gravity = 1;
		for(unsigned i = 0; i < mags.size(); i++){
			auto& p = processing_data[i];
			std::get<0>(p).calculate(bufs->bufs[i]);
			std::get<0>(p).magnitudes(mags[i], 1);
			Processing::calculate_gravity(mags[i], std::get<1>(p), gravity, dt);
		}

		// update buffer
		draw_buf->update_fft(mags);

		// draw
		glClear(GL_COLOR_BUFFER_BIT);
		for(auto& renderer : renderers){
			renderer.draw();
		}

		window.swapBuffers();
	}
	while(!window.shouldClose() && running);

	return 0;
}

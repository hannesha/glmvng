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

int main() {
	std::signal(SIGINT, sigint_handler);
	auto window = GLXwindow();

	window.setTitle("window title");

	// load extensions
	GL::init();

	Config cfg("config");

	// init buffers
	// init audio stream

	std::shared_ptr<DrawBuffer> draw_buf = std::make_shared<DrawBuffer>();
	GL::get_error("create");
	Renderer rend(cfg.renderers[0], draw_buf);
	GL::get_error("create render");
	// init fft
	Buffers<float>::Ptr bufs(new Buffers<float>());
	bufs->bufs.emplace_back(cfg.input.buffer_len);
	const int fft_len = 4096;
	FFT fft(fft_len);
	std::vector<Magnitudes> mags(1, Magnitudes(fft_len));
	Processing::GravityInfo gravity_info(fft_len);

//	std::vector<float> test_data(100);
//	for(int x = 0; x < 100; x++){
//		test_data[x] = std::sin(static_cast<float>(x)/10);
//	}
//	bufs.bufs[0].write(test_data);
//	draw_buf->update(bufs.bufs);
	GL::get_error("update");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// generate Renderers from config
	Input::Ptr input(new Pulse_Async(bufs));
	input->start_stream(cfg.input);

	// mainloop
	glClearColor(0.0, 0.0, 0.0, 0.8);
	float old_rms = 0.5;
	float rms_mix = 0.8;
	do{
		window.pollEvents();
		float rms;
		{
			rms = std::sqrt(Processing::rms(bufs->bufs[0]) / bufs->bufs[0].size());
		}
		old_rms = old_rms * (1.f - rms_mix) + rms * rms_mix;
		ShaderConfig::value_type vrms = {"rms", Scalar(old_rms)};
		rend.set_uniform(vrms);
		draw_buf->update(bufs->bufs);

		fft.calculate(bufs->bufs[0]);
		fft.magnitudes(mags[0], 1);
		Processing::calculate_gravity(mags[0], gravity_info, 1., 0.016);
		draw_buf->update_fft(mags);


		glClear(GL_COLOR_BUFFER_BIT);
		rend.draw();

		window.swapBuffers();
	}
	while(!window.shouldClose() && running);

	return 0;
}

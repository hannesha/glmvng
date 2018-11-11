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

#include <cmath>

int main() {
	auto window = GLFWWindow();

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
	bufs->bufs.emplace_back(cfg.renderers[0].output_size);

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
	Module_Config::Input input_cfg;
	input->start_stream(input_cfg);

	// mainloop
	glClearColor(0.0, 0.0, 0.0, 0.0);
	float old_rms = 0.5;
	float rms_mix = 0.8;
	do{
		window.pollEvents();
		float rms;
		{
			rms = std::sqrt(Processing::rms(bufs->bufs[0]) / bufs->bufs[0].size_());
		}
		old_rms = old_rms * (1.f - rms_mix) + rms * rms_mix;
		ShaderConfig::value_type vrms = {"rms", Scalar(old_rms)};
		rend.set_uniform(vrms);
		draw_buf->update(bufs->bufs);


		glClear(GL_COLOR_BUFFER_BIT);
		rend.draw();

		window.swapBuffers();
	}
	while(!window.shouldClose());

	return 0;
}

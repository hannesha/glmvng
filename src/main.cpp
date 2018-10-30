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

#include <cmath>

int main() {
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
	do{
		draw_buf->update(bufs->bufs);


		glClear(GL_COLOR_BUFFER_BIT);
		rend.draw();

		window.swapBuffers();
	}
	while(!window.shouldClose());

	return 0;
}

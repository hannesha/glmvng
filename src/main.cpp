#include <iostream>
#include <string>
#include <stdexcept>

#include "GLFW.hpp"

#include "Config.hpp"
#include "Drawbuffer.hpp"


int main() {
	GLFW glfw;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(480, 320, "test", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// load extensions
	GL::init();

	Config cfg("config");

	// init buffers
	// init audio stream

	DrawBuffer draw_buf();
	// init fft

	// generate Renderers from config



	// mainloop

	return 0;
}

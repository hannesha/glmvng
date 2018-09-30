#include <iostream>
#include <string>
#include <stdexcept>


#include "GL_utils.hpp"
#include <GLFW/glfw3.h>

#include "Config.hpp"
#include "Drawbuffer.hpp"

// glfw raii wrapper
class GLFW {
public:
	GLFW() { if(!glfwInit()) throw std::runtime_error("GLFW init failed!"); };

	~GLFW() { glfwTerminate(); };
};

int main() {
	GLFW glfw;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(480, 320, "test", nullptr, nullptr);
	glfwMakeContextCurrent(window);

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

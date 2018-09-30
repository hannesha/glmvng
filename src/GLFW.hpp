#pragma once

#include "GL_utils.hpp"
#include <GLFW/glfw3.h>

// glfw raii wrapper
class GLFW {
public:
	GLFW() { if(!glfwInit()) throw std::runtime_error("GLFW init failed!"); };

	~GLFW() { glfwTerminate(); };
};

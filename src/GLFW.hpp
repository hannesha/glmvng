#pragma once

#include "GL_utils.hpp"
#include <GLFW/glfw3.h>
#include <string>

class GLFWWindow {
public:
	GLFWWindow(); //TODO use window config
	~GLFWWindow();

	void swapBuffers();
	void pollEvents();
	bool shouldClose();

	void setTitle(const std::string& title){
		glfwSetWindowTitle(window, title.c_str());
	}

private:
	GLFWwindow* window;
	static void resizeCB(GLFWwindow* window, int w, int h);
};

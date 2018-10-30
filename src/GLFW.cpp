#include "GLFW.hpp"
#include <stdexcept>

GLFWWindow::GLFWWindow(){
	if(!glfwInit()){
		throw std::runtime_error("GLFW init failed!");
	}

	//set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(640, 480, "test", nullptr, nullptr);
	if(!window){
		glfwTerminate();
		throw std::runtime_error("Unable to create glfw window!");
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resizeCB);
}

GLFWWindow::~GLFWWindow(){
	glfwTerminate();
}

void GLFWWindow::swapBuffers(){
	glfwPollEvents();
	glfwSwapBuffers(window);
}

bool GLFWWindow::shouldClose(){
	return glfwWindowShouldClose(window);
}

void GLFWWindow::resizeCB(GLFWwindow* window, int w, int h){
	glViewport(0, 0, w, h);
}

#include "GLFW.hpp"
#include <stdexcept>

GLFWWindow::GLFWWindow():
	callback([](int h, int w){})
{
	if(!glfwInit()){
		throw std::runtime_error("GLFW init failed!");
	}

	//set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef GLFW_TRANSPARENT_FRAMEBUFFER
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
#endif

	window = glfwCreateWindow(640, 480, "test", nullptr, nullptr);
	if(!window){
		glfwTerminate();
		throw std::runtime_error("Unable to create glfw window!");
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, resizeCB);
}

GLFWWindow::~GLFWWindow(){
	glfwTerminate();
}

void GLFWWindow::pollEvents(){
	glfwPollEvents();
}

void GLFWWindow::swapBuffers(){
	glfwSwapBuffers(window);
}

bool GLFWWindow::shouldClose(){
	return glfwWindowShouldClose(window);
}

void GLFWWindow::resizeCB(GLFWwindow* window, int w, int h){
	GLFWWindow* obj = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
	obj->callback(w, h);
	glViewport(0, 0, w, h);
}

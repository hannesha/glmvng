#pragma once

#include "GL_utils.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

class GLFWWindow {
public:
	using CBFunction = std::function<void(int, int)>;

	GLFWWindow(); //TODO use window config
	~GLFWWindow();

	void swapBuffers();
	void pollEvents();
	bool shouldClose();

	void setTitle(const std::string& title){
		glfwSetWindowTitle(window, title.c_str());
	}

	void setResizeCallback(const CBFunction& cb){
		callback = cb;
	}

	const int getWidth(){
		int width;
		glfwGetWindowSize(window, &width, nullptr);
		return width;
	}
	const int getHeight(){
		int height;
		glfwGetWindowSize(window, nullptr, &height);
		return height;
	}

private:
	CBFunction callback;

	GLFWwindow* window;
	static void resizeCB(GLFWwindow* window, int w, int h);
};

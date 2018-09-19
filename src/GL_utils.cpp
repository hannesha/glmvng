/*
 *	Copyright (C) 2016  Hannes Haberl
 *
 *	This file is part of GLMViz.
 *
 *	GLMViz is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	GLMViz is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with GLMViz.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GL_utils.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

using namespace GL;

Program::Program() {
	id = glCreateProgram();

	if(id == 0) {
		throw std::runtime_error("Failed to create shader program!");
	}
}

void Program::check_link_status() {
	// check link status and throw if the shaders can't be linked
	GLint link_status;
	glGetProgramiv(id, GL_LINK_STATUS, &link_status);

	if(link_status == GL_FALSE) {
		GLint length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> log(length);
		glGetProgramInfoLog(id, length, &length, log.data());

		std::string err(log.begin(), log.end());
		throw std::invalid_argument(err);
	}
}

Shader::Shader(const char* code, GLuint type) {
	id = glCreateShader(type);

	if(id == 0) {
		throw std::runtime_error("Failed to create shader!");
	}

	glShaderSource(id, 1, &code, nullptr);
	glCompileShader(id);

	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if(status == GL_FALSE) {
		GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> log(length);
		glGetShaderInfoLog(id, length, &length, log.data());

		std::string err(log.begin(), log.end());
		//std::cout << err << std::endl;
		throw std::invalid_argument(err);
	}
}

void GL::get_error(const char* str) {
	GLenum err = glGetError();
	std::string err_str;

	switch(err) {
	case GL_INVALID_OPERATION:
		err_str = "INVALID_OPERATION";
		break;

	case GL_INVALID_ENUM:
		err_str = "INVALID_ENUM";
		break;

	case GL_INVALID_VALUE:
		err_str = "INVALID_VALUE";
		break;

	case GL_OUT_OF_MEMORY:
		err_str = "OUT_OF_MEMORY";
		break;

	case GL_INVALID_FRAMEBUFFER_OPERATION:
		err_str = "INVALID_FRAMEBUFFER_OPERATION";
		break;

	default:
		return;
	}

	std::cout << str << " :" << err_str << std::endl;
}

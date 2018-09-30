#include "GL_utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Scalar.hpp"
#include <map>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "Config.hpp"

using Shader_config = std::vector<std::pair<std::string, Scalar>>;

std::string load_source(const std::string& path) {
	std::ifstream file(path);
	file.exceptions(std::ifstream::failbit);
	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

void set_uniform(const Shader_config::value_type& val, GL::Program& sh) {
	GLint loc = sh.get_uniform(val.first.c_str());
	//std::cout << loc << std::endl;
	if(loc < 0) {
		// log unused value
		std::cout << "Ignoring: " << val.first << std::endl;
		return;
	}

	switch(val.second.type) {
	case Scalar::Type::INT:
		std::cout << "loc: " << loc << " setting int: " << val.first << std::endl;
		glUniform1i(loc, val.second.value.i);
		GL::get_error("set int");
		break;

	case Scalar::Type::FLOAT:
		std::cout << "loc: " << loc << " setting float: " << val.first << std::endl;
		glUniform1f(loc, val.second.value.f);
		GL::get_error("set float");
		break;

	default:
		std::cout << "ignoring value: " << val.first << std::endl;
	}
}

//void config_shader(const Shader_config& cfg, GL::Program& sh) {
//	sh();
//
//	for(auto& entry : cfg) {
//		set_uniform(entry, sh);
//	}
//}

// glfw raii wrapper
class GLFW {
public:
	GLFW() { if(!glfwInit()) throw std::runtime_error("GLFW init failed!"); };

	~GLFW() { glfwTerminate(); };
};

/*GL::Shader load_shader(const std::string& path, const std::string& default_source, GLenum type){
	std::string source;
	try{
		source = load_source(path);
	}
	catch(std::ios_base::failure& e){
		std::cerr << "Shader source: " << path << " can't be loaded!" << std::endl;
		source = default_source;
	}

	GL::Shader shader(source.c_str(), type);
	return shader;
}*/


template<typename K, typename V>
void dump_map(const std::map<K, V>& m) {
	for(const auto& i : m) {
		std::cout << "[" << i.first << "]: " << i.second << std::endl;
	}
}


// add shader_map (compiled shaders)
using Shader_source_map = std::map<std::string, std::pair<std::string, GLenum>>;

void add_shader(std::vector<GL::Shader>& shaders, const Shader_source_map::mapped_type& shader_source) {
	const std::string& source = shader_source.first;
	GLenum type = shader_source.second;
	shaders.emplace_back(source.c_str(), type);
} 

void make_shader_config(libconfig::Setting& setting, Shader_config& sh_config){
	for(auto& s : setting) {
		if(!s.isNumber()) {
			continue;
		}

		std::string name = s.getName();
		//sh_config.emplace_back(name, setting_to_scalar(s));
	}
}

int main() {
	//std::string test = load_source("test.txt");
	//std::cout << test << std::endl;
	GLFW glfw;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(480, 320, "test", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	GL::init();

//	std::vector<GL::Shader> shs;
//	Shader_source_map sh_sources;
//	std::string filename = "vertex.glsl";
//	sh_sources.emplace(filename, std::make_pair(load_source(filename), get_shader_type(filename)));
//
//	filename = "test.frag";
//	sh_sources.emplace(filename, std::make_pair(load_source(filename), get_shader_type(filename)));
//	add_shader(shs, sh_sources["vertex.glsl"]);
//	add_shader(shs, sh_sources["test.frag"]);
//	GL::Program gl_shader;
//	//gl_shader.link(shs[0], shs[1]);
//	gl_shader.link_vector(shs);
//	GL::get_error("test");

	Config cfg("config");


	//Shader_config scalar_settings;
	//make_shader_config(cfg.getRoot(), scalar_settings);

	//config_shader(scalar_settings, gl_shader);
	//dump_map(scalar_settings);
	return 0;
}

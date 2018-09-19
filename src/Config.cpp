#include "Config.hpp"

#include "xdg.hpp"
//#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>

const std::string CONFIG_FOLDER("/glmvng/");
const std::string CONFIG_FILE("config");
const std::string CONFIG_PATH(CONFIG_FOLDER + CONFIG_FILE);
const std::string DEFAULT_CONFIG_PATH("/etc" + CONFIG_FOLDER + CONFIG_FILE);


Config::Config(const std::string& config_file){
	if(!config_file.empty()){
		if(xdg::verify_path(config_file)){
			file = config_file;
		}else{
			file = xdg::find_config(CONFIG_FOLDER + config_file);
			if (file.empty()){
				std::cerr << "The specified config file doesn't exist, falling back to default config!" << std::endl;
			}
		}
	}

	if(file.empty()) file = xdg::find_config(CONFIG_PATH);

	if(file.empty()) file = DEFAULT_CONFIG_PATH;

	reload();
}

void Config::reload(){
	try{
		cfg.readFile(file.c_str());

		// parse

		// iterate over RendererN, parse uniforms
		unsigned index = 1;
		while(true){
			std::string path = "Renderer" + std::to_string(index);
			bool setting_exists = cfg.exists(path);

			if(!setting_exists){
				break;
			}

			// use render config vector
			if(renderers.size() > index){
				renderers[index -1] = parse_renderer(path);
			}else{
				renderers.push_back(parse_renderer(path));
			}
			index++;
		}
		
	}catch(const libconfig::FileIOException &fioex){
		std::cerr << "I/O error while reading file." << std::endl;

		std::cout << "Using default settings." << std::endl;
	}catch(const libconfig::ParseException &pex){
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << std::endl;

		std::cout << "Using default settings." << std::endl;
	}
}

//TODO generate renderconfig
RenderConfig Config::parse_renderer(const std::string& path){
	RenderConfig config;
	//TODO parse base renderer settings
	//TODO parse draw type, compute buf size ??
	
		
	ShaderConfig uniforms;
	parse_uniforms(cfg.lookup(path), uniforms);
	config.uniforms = uniforms;
	return config;
}

Scalar Config::setting_to_scalar(libconfig::Setting& s) {
	switch(s.getType()) {
	case libconfig::Setting::TypeFloat:
		return Scalar(static_cast<float>(s));

	case libconfig::Setting::TypeInt:
		return Scalar(static_cast<int>(s));

	case libconfig::Setting::TypeInt64:
		return Scalar(static_cast<long long>(s));

	default:
		throw std::invalid_argument("Wrong setting type");
	}
}

GLenum Config::get_shader_type(const std::string& filename){
	auto pos = filename.rfind("vert");
	if(pos != std::string::npos){
		return GL_VERTEX_SHADER;
	}

	pos = filename.rfind("frag");
	if(pos != std::string::npos){
		return GL_FRAGMENT_SHADER;
	}

	pos = filename.rfind("geom");
	if(pos != std::string::npos){
		return GL_GEOMETRY_SHADER;
	}

	throw std::invalid_argument("Can't parse shader type");
}

void Config::parse_uniforms(libconfig::Setting& root, ShaderConfig& sh_config){
	for(auto& s : root) {
		if(!s.isNumber()) {
			continue;
		}

		std::string name = s.getName();
		sh_config.emplace(name, setting_to_scalar(s));
	}
}

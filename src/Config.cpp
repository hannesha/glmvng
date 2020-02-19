#include "Config.hpp"

#include "xdg.hpp"
//#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "Utils.hpp"

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

		// parse input settings

		// clear shader cache
		shaders.clear();
		parse_input(input, "Input");

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

		std::cout << "Aborting." << std::endl;
		throw fioex;
	}catch(const libconfig::ParseException &pex){
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << std::endl;

		std::cout << "Using default settings." << std::endl;
	}
}

RenderConfig Config::parse_renderer(const std::string& path){
	RenderConfig config;

	auto config_dir_end = file.rfind("/");
	if(config_dir_end == std::string::npos){
		config_dir_end = 0;
	} else {
		config_dir_end++;
	}

	std::string shader_path(file, 0, config_dir_end);
	shader_path += "shaders/";

	libconfig::Setting& cfg_shaders = cfg.lookup(path + ".shaders");
	if(!cfg_shaders.isArray()){
		throw std::invalid_argument("Expected shader name array at: " + path);
	}

	for(auto& s : cfg_shaders){
		std::string shader_file = shader_path + (const char *)s;
		auto shader_ptr = load_shader(shader_file);
		config.shaders.push_back(shader_ptr);
	}
	
	if(!cfg.lookupValue(path + ".output_size", config.output_size)){
		config.output_size = input.buffer_len;
	}
	config.drawtype = parse_drawtype(cfg.lookup(path + ".drawtype"));

	try{
		parse_uniforms(cfg.lookup(path + ".uniforms"), config.uniforms);
		parse_uniform_vectors(cfg.lookup(path + ".uniforms.vectors"), config.vectors);
	} catch (libconfig::SettingNotFoundException& e) {}

	config.uniforms.emplace("output_size_1", 1.f/config.output_size);
	config.uniforms.emplace("buffer_size", (int)input.buffer_len);
	return config;
}

bool Config::rfind(const std::string& input, const std::string& compare){
	return input.rfind(compare) != std::string::npos;
}

GLenum Config::parse_drawtype(const char* name){
	std::string str_name(name);
	std::transform(str_name.begin(), str_name.end(), str_name.begin(), [](char c){ return std::tolower(c); });
	if(rfind(str_name, "triangle strip")){
		return GL_TRIANGLE_STRIP;
	}
	else if(rfind(str_name, "triangles")){
		return GL_TRIANGLES;
	}
	else if(rfind(str_name, "line strip")){
		return GL_LINE_STRIP;
	}
	else if(rfind(str_name, "lines")){
		return GL_LINES;
	}
	else if(rfind(str_name, "points")){
		return GL_POINTS;
	}
	
	throw std::invalid_argument("unknown draw type");
}

std::shared_ptr<GL::Shader> Config::load_shader(const std::string& path){
	auto shader = shaders.find(path);
	if(shader != shaders.end()){
		return shader->second;
	}

	// shader not found in map
	auto shader_ptr = std::make_shared<GL::Shader>(
		Utils::load_source(path).c_str(),
		get_shader_type(path)
	);
	shaders.emplace(path, shader_ptr);
	return shader_ptr;
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

void Config::parse_uniform_vectors(libconfig::Setting& root, ShaderVectors& vectors){
	for(auto& s : root){
		std::string name = s.getName();
		if(name.rfind("_color") == std::string::npos){
			// TODO parse vector
			continue;
		}
		try{
			Vec4 color = parse_color(s);
			vectors.emplace(name, color);
		}catch(std::logic_error& e){
			continue;
		}
	}
}

Vec4 Config::parse_color(libconfig::Setting& setting){
	Vec4 cres; 
	std::string color;
	color = setting.c_str();
	if(color[0] == '#'){
		color = color.substr(1);
	}
	long value = std::stol(color, nullptr, 16);
	if(color.length() == 8){
		// color with alpha value
		cres[3] = static_cast<float>((value / 0x1000000) % 0x100) / 255.;
	}else{
		// opaque color
		cres[3] = 1;
	}
	cres[0] = static_cast<float>((value / 0x10000) % 0x100) / 255.;
	cres[1] = static_cast<float>((value / 0x100) % 0x100) / 255.;
	cres[2] = static_cast<float>(value % 0x100) / 255.;
	return cres;
}

void Config::parse_input(Module_Config::Input& i, const std::string& path){
	std::string str_source;
	cfg.lookupValue(path + ".source", str_source);
	// convert string to lowercase and evaluate source
	std::transform(str_source.begin(), str_source.end(), str_source.begin(), ::tolower);
	if(str_source == "pulse"){
		i.source = Module_Config::Source::PULSE;
	}else{
		i.source = Module_Config::Source::FIFO;
	}

	cfg.lookupValue(path + ".file", i.file);
	cfg.lookupValue(path + ".device", i.device);
	cfg.lookupValue(path + ".stereo", i.stereo);
	cfg.lookupValue(path + ".f_sample", i.f_sample);
	cfg.lookupValue(path + ".fft_size", i.fft_size);
	int buffer_duration;
	cfg.lookupValue(path + ".buffer_duration", buffer_duration);
	i.buffer_len = i.f_sample * buffer_duration / 1000;

	if(i.buffer_len <= 0){
		i.buffer_len = 4096;
	}
}

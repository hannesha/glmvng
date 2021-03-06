#pragma once

#include <libconfig.h++>
#include "Module_Config.hpp"
//#include "Scalar.hpp"
#include <memory>
#include "RenderConfig.hpp"
#include "GL_utils.hpp"

class Config {
	public:
		Config(const std::string&);
		// disable copy construction
		Config(const Config&) = delete;
		void reload();


		std::string get_file(){
			return file;
		}
		
		Scalar setting_to_scalar(libconfig::Setting&);
		//TODO move to glutils, sync glutils
		GLenum get_shader_type(const std::string&);

		std::vector<RenderConfig> renderers;
		Module_Config::Input input;

	private:
		RenderConfig parse_renderer(const std::string&);
		GLenum parse_drawtype(const char*);
		static bool rfind(const std::string&, const std::string&);
		void parse_uniforms(libconfig::Setting&, ShaderConfig&);
		void parse_uniform_vectors(libconfig::Setting&, ShaderVectors&);
		Vec4 parse_color(libconfig::Setting&);
		void parse_input(Module_Config::Input&, const std::string&);
		std::shared_ptr<GL::Shader> load_shader(const std::string& path);
		
		libconfig::Config cfg;
		std::string file;
		std::map<std::string, std::shared_ptr<GL::Shader>> shaders;
};

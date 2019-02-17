// takes a render config as input
// render config contains all the shaders and settings (maybe additional config with limits)
// the program is linked on construction
// configure method -- takes a renderconfig and sets the uniforms
#pragma once

#include "GL_utils.hpp"
#include "RenderConfig.hpp"
#include <memory>

class DrawBuffer;

class Renderer{
public:
	Renderer(const RenderConfig&, std::shared_ptr<DrawBuffer>&);
	~Renderer(){};

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = default;

	void draw();

	void set_uniform(const ShaderConfig::value_type& val){
		shader();
		set_uniform(val, shader);
	}

private:
	const RenderConfig& renderconfig;
	std::shared_ptr<DrawBuffer> drawbuffer;

	GL::Program shader;
	GL::VAO vao; // blank vao(needed for drawing)

	void link_shaders(const RenderConfig::Shaders&);
	void configure();
	void set_uniform(const ShaderConfig::value_type& val, GL::Program& sh);
};

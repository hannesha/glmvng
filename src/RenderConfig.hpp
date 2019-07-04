// contains uniform map (Scalar map)
// contains references to shaders (shaders are kept in config)
// holds additional attributes (transformation, outputsize, etc..)
// draw type enum

#pragma once

#include "GL_utils.hpp"
#include "Scalar.hpp"
#include <vector>
#include <utility>
#include <memory>
#include "ShaderConfig.hpp"

using Shader_ptr = std::shared_ptr<GL::Shader>;

struct RenderConfig{
	using Shaders = std::vector<Shader_ptr>;

	unsigned output_size;
	GLenum drawtype = GL_LINE_STRIP;
	// Shaderconfig
	ShaderConfig uniforms;
	ShaderVectors vectors;

	//TODO use shader map instead and store the keys in that vector
	Shaders shaders;
	// use float ?
	std::vector<double> transformation;
};

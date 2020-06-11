#include "Renderer.hpp"
#include "Drawbuffer.hpp"
#include <iostream>

Renderer::Renderer(const RenderConfig& renderconf, std::shared_ptr<DrawBuffer>& draw_buf): renderconfig(renderconf), drawbuffer(draw_buf)
{
	link_shaders(renderconfig.shaders);

	configure();
}

void Renderer::link_shaders(const RenderConfig::Shaders& shaders){
	for(auto sh : shaders){
		shader.attach(*sh);
	}
	glLinkProgram(shader.get_id());
	for(auto sh : shaders){
		shader.detach(*sh);
	}
	shader.check_link_status();
GLDEBUG;
}

void Renderer::draw(){
	shader();
	vao();
GLDEBUG;
	
	DrawBuffer::TextureHandles textures = drawbuffer->get_handles();
	glActiveTexture(GL_TEXTURE0);
	textures.t_left(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE1);
	textures.t_right(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE2);
	textures.t_f_left(GL_TEXTURE_BUFFER);
GLDEBUG;

	glActiveTexture(GL_TEXTURE3);
	textures.t_f_right(GL_TEXTURE_BUFFER);
GLDEBUG;

	glDrawArrays(renderconfig.drawtype, 0, renderconfig.output_size);
GLDEBUG;

	GL::VAO::unbind();
	GL::Texture::unbind(GL_TEXTURE_BUFFER);
}

void Renderer::set_uniform(const ShaderConfig::value_type& val, GL::Program& sh) {
	GLint loc = sh.get_uniform(val.first.c_str());
	//std::cout << loc << std::endl;
GLDEBUG;
	if(loc < 0) {
		// log unused value
		//std::cout << "Ignoring: " << val.first << std::endl;
		return;
	}

	std::visit([&](auto&& value) {
		using T = std::decay_t<decltype(value)>;
		if constexpr (std::is_same_v<T, int>)
		{
			//std::cout << "loc: " << loc << " setting int: " << value << std::endl;
			glUniform1i(loc, value);
			GLDEBUG;
		}
		else
		{
			//std::cout << "loc: " << loc << " setting float: " << value << std::endl;
			glUniform1f(loc, value);
			GLDEBUG;
		}
	}, val.second);
}

void Renderer::set_vector(const ShaderVectors::value_type& vec, GL::Program& sh) {
	GLint loc = sh.get_uniform(vec.first.c_str());
	//std::cout << loc << std::endl;
GLDEBUG;
	if(loc < 0) {
		// log unused value
		//std::cout << "Ignoring: " << val.first << std::endl;
		return;
	}

	glUniform4fv(loc, 1, vec.second.data());
}

void Renderer::configure(){
	shader();
	// set texture buffer locations
	glUniform1i(shader.get_uniform("t_left"), 0);
	glUniform1i(shader.get_uniform("t_right"), 1);
	glUniform1i(shader.get_uniform("t_f_left"), 2);
	glUniform1i(shader.get_uniform("t_f_right"), 3);

	for(auto& uniform : renderconfig.uniforms){
		set_uniform(uniform, shader);
	}
GLDEBUG;
	for(auto& vector : renderconfig.vectors){
		set_vector(vector, shader);
	}
GLDEBUG;
}
